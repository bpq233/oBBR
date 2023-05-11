#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <ngx_event_quic_connection.h>

#define CUBIC_FAST_CONVERGENCE  1
#define CUBIC_MSS               1460
#define CUBIC_BETA              718     /* 718/1024=0.7 */
#define CUBIC_BETA_SCALE        1024
#define CUBIC_C                 410     /* 410/1024=0.4 */
#define CUBE_SCALE              40u     /* 2^40=1024 * 1024^3 */
#define CUBIC_TIME_SCALE        10u
#define CUBIC_MAX_SSTHRESH      0xFFFFFFFF

#define CUBIC_MIN_WIN           (10 * CUBIC_MSS)
#define CUBIC_MAX_INIT_WIN      (100 * CUBIC_MSS)
#define CUBIC_INIT_WIN          (10 * CUBIC_MSS)
#define MICROS_PER_SECOND 1000

#define _min(a, b) ((a) < (b) ? (a) : (b))
#define _max(a, b) ((a) > (b) ? (a) : (b))

const uint64_t cube_factor =
    (1ull << CUBE_SCALE) / CUBIC_C / CUBIC_MSS;

/*
 * Compute congestion window to use.
 * W_cubic(t) = C*(t-K)^3 + W_max (Eq. 1)
 * K = cubic_root(W_max*(1-beta_cubic)/C) (Eq. 2)
 * t: the time difference between the current time and the last window reduction
 * K: the time period for the function to grow from W to Wmax
 * C: window growth factor
 * beta: window reduction factor
 */

void CubicInit(Cubic *cubic) {
    cubic->init_cwnd = CUBIC_INIT_WIN;
    cubic->epoch_start = 0;
    cubic->cwnd = CUBIC_INIT_WIN;
    cubic->tcp_cwnd = CUBIC_INIT_WIN;
    cubic->last_max_cwnd = CUBIC_INIT_WIN;
    cubic->ssthresh = CUBIC_MAX_SSTHRESH;
    cubic->congestion_recovery_start_time = 0;
}

bool CubicInCongestionRecovery(Cubic *cubic, ngx_msec_t sent_time) {
    return sent_time <= cubic->congestion_recovery_start_time;
}

void CubicUpdate(Cubic *cubic, uint64_t acked_bytes, ngx_msec_t now) {
    uint64_t        t;      /* unit: ms */
    uint64_t        offs;   /* offs = |t - K| */
    uint64_t        delta, bic_target;  /* delta = C*(t-K)^3 */

    /* First ACK after a loss event. */
    if (cubic->epoch_start == 0) {
        cubic->epoch_start = now;

        /* take max(last_max_cwnd, cwnd) as current Wmax origin point */
        if (cubic->cwnd >= cubic->last_max_cwnd) {
            /* exceed origin point, use cwnd as the new point */
            cubic->bic_K = 0;
            cubic->bic_origin_point = cubic->cwnd;

        } else {
            /*
             * K = cubic_root(W_max*(1-beta_cubic)/C) = cubic_root((W_max-cwnd)/C)
             * cube_factor = (1ull << NGX_CUBE_SCALE) / NGX_CUBIC_C / NGX_MSS
             *             = 2^40 / (410 * MSS) = 2^30 / (410/1024*MSS)
             *             = 2^30 / (C*MSS)
             */
            //cubic->bic_K = cbrt(cube_factor * (cubic->last_max_cwnd - cubic->cwnd));
            double l = 0, r = 10000, n = cube_factor * (cubic->last_max_cwnd - cubic->cwnd);
            while(r - l > 1e-4)
            {
                double mid = (l + r) / 2;
                if(mid * mid * mid < n) l = mid;
                else r = mid;
            }
            cubic->bic_K = l;
            cubic->bic_origin_point = cubic->last_max_cwnd;
        }
    }

    /*
     * t = elapsed_time * 1024 / 1000000, convert microseconds to milliseconds,
     * multiply by 1024 in order to be able to use bit operations later.
     */
    t = (now + cubic->min_rtt - cubic->epoch_start) << CUBIC_TIME_SCALE / MICROS_PER_SECOND;

    /* calculate |t - K| */
    if (t < cubic->bic_K) {
        offs = cubic->bic_K - t;

    } else {
        offs = t - cubic->bic_K;
    }

    /* 410/1024 * off/1024 * off/1024 * off/1024 * MSS */
    delta = (CUBIC_C * offs * offs * offs * CUBIC_MSS) >> CUBE_SCALE;

    if (t < cubic->bic_K) {
        bic_target = cubic->bic_origin_point - delta;

    } else {
        bic_target = cubic->bic_origin_point + delta;
    }

    /* the maximum growth rate of CUBIC is 1.5x per RTT, i.e. 1 window every 2 ack. */
    bic_target = _min(bic_target, cubic->cwnd + acked_bytes / 2);

    /* take the maximum of the cwnd of TCP reno and the cwnd of cubic */
    bic_target = _max(cubic->tcp_cwnd, bic_target);

    if (bic_target == 0) {
        bic_target = cubic->init_cwnd;
    }

    cubic->cwnd = bic_target;
}

void CubicOnLost(Cubic *cubic, ngx_msec_t sent_time) {
    /* No reaction if already in a recovery period. */
    if (CubicInCongestionRecovery(cubic, sent_time)) {
        return;
    }
    cubic->congestion_recovery_start_time = ngx_current_msec;
    cubic->epoch_start = 0;

    /* should we make room for others */
    if (CUBIC_FAST_CONVERGENCE && cubic->cwnd < cubic->last_max_cwnd) {
        /* (1.0f + NGX_CUBIC_BETA) / 2.0f convert to bitwise operations */
        cubic->last_max_cwnd = cubic->cwnd * (CUBIC_BETA_SCALE + CUBIC_BETA) / (2 * CUBIC_BETA_SCALE);

    } else {
        cubic->last_max_cwnd = cubic->cwnd;
    }

    /* Multiplicative Decrease */
    cubic->cwnd = cubic->cwnd * CUBIC_BETA / CUBIC_BETA_SCALE;
    cubic->cwnd = _max(cubic->cwnd, CUBIC_MIN_WIN);
    cubic->tcp_cwnd = cubic->cwnd;
    cubic->ssthresh = cubic->cwnd;
}

void CubicOnAck(Cubic *cubic, uint64_t acked_bytes, ngx_msec_t sent_time, ngx_msec_t now) {
    ngx_msec_t  rtt = now - sent_time;

    if (cubic->min_rtt == 0 || rtt < cubic->min_rtt) {
        cubic->min_rtt = rtt;
    }

    /* Do not increase congestion window in recovery period. */
    if (CubicInCongestionRecovery(cubic, sent_time)) {
        return;
    }

    if (cubic->cwnd < cubic->ssthresh) {
        /* slow start */
        cubic->tcp_cwnd += acked_bytes;
        cubic->cwnd += acked_bytes;

    } else {
        /* congestion avoidance */
        cubic->tcp_cwnd += CUBIC_MSS * CUBIC_MSS / cubic->tcp_cwnd;
        CubicUpdate(cubic, acked_bytes, now);
    }
}

uint64_t CubicGetCwnd(Cubic *cubic) {
    return cubic->cwnd;
}

void CubicReset(Cubic *cubic) {
    cubic->epoch_start = 0;
    cubic->cwnd = CUBIC_MIN_WIN;
    cubic->tcp_cwnd = CUBIC_MIN_WIN;
    cubic->last_max_cwnd = CUBIC_MIN_WIN;
}

int32_t CubicInSlowStart(Cubic *cubic) {
    return cubic->cwnd < cubic->ssthresh ? 1 : 0;
}