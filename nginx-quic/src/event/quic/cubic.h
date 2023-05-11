#ifndef _CUBIC_H_INCLUDED_
#define _CUBIC_H_INCLUDED_

#include <ngx_config.h>
#include <ngx_core.h>
#include <stdbool.h>


typedef struct {
    uint64_t        init_cwnd;          /* initial window size in MSS */
    uint64_t        cwnd;               /* current window size in bytes */
    uint64_t        tcp_cwnd;           /* cwnd calculated according to Reno's algorithm */
    uint64_t        last_max_cwnd;      /* last max window size */
    uint64_t        ssthresh;           /* slow start threshold */
    uint64_t        bic_origin_point;   /* Wmax origin point */
    uint64_t        bic_K;              /* time period from W growth to Wmax */
    ngx_msec_t      epoch_start;        /* the moment when congestion switchover begins, in microseconds */
    ngx_msec_t      min_rtt;
    ngx_msec_t      congestion_recovery_start_time;
}Cubic;

void CubicInit(Cubic *cubic);
bool CubicInCongestionRecovery(Cubic *cubic, ngx_msec_t sent_time);
void CubicUpdate(Cubic *cubic, uint64_t acked_bytes, ngx_msec_t now);
void CubicOnLost(Cubic *cubic, ngx_msec_t sent_time);
void CubicOnAck(Cubic *cubic, uint64_t acked_bytes, ngx_msec_t sent_time, ngx_msec_t now);
uint64_t CubicGetCwnd(Cubic *cubic);
void CubicReset(Cubic *cubic);
int32_t CubicInSlowStart(Cubic *cubic);

#endif