#include <ngx_event_quic_connection.h>

#define NGX_kInitialRtt 250
#define NGX_MIN_BURST_NUM (2 * NGX_QUIC_MSS)
#define NGX_MAX_BURST_NUM (10 * NGX_QUIC_MSS)
#define TRUE 1
#define FALSE 0
#define NGX_CLOCK_GRANULARITY_MS 3
#define NGX_PACING_DELAY_MS NGX_CLOCK_GRANULARITY_MS

void
ngx_pacing_init(ngx_pacing_t *pacing, int pacing_on, ngx_connection_t *c)
{
    pacing->bytes_budget = NGX_MAX_BURST_NUM;
    pacing->last_sent_time = 0;
    pacing->pacing_on = pacing_on;
    pacing->pending_budget = 0;
}

uint64_t
ngx_pacing_rate_calc(ngx_pacing_t *pacing, ngx_connection_t *c)
{
    ngx_quic_congestion_t  *cg;
    ngx_quic_connection_t  *qc;

    qc = ngx_quic_get_connection(c);
    cg = &qc->congestion;

    if (ngx_strcmp(ngx_cong, "CUBIC") == 0) {
        int pacing_rate = cg->window * 1000 / qc->avg_rtt;
        if (CubicInSlowStart(&cg->cubic)) {
            pacing_rate = 2 * pacing_rate;
        } else {
            pacing_rate = 1.2 * pacing_rate;
            //printf("%d\n", pacing_rate);
        }
        //printf("%d\n", pacing_rate);
        return pacing_rate;
    }
    if (ngx_strcmp(ngx_cong, "BBRv2") == 0) {
        return cg->bbr2.pacing_rate;
    }
    return cg->bbr.pacing_rate;
}

static uint32_t 
ngx_pacing_max_burst_size(ngx_pacing_t *pacing, ngx_connection_t *c)
{
    ngx_msec_t t_diff = (NGX_PACING_DELAY_MS + NGX_CLOCK_GRANULARITY_MS);
    uint64_t max_burst_bytes = t_diff * ngx_pacing_rate_calc(pacing, c) 
                                / 1000;
    return ngx_max(NGX_MAX_BURST_NUM, max_burst_bytes);
}

static uint32_t 
ngx_pacing_calc_budget(ngx_pacing_t *pacing, ngx_msec_t now, ngx_connection_t *c)
{
    uint32_t budget = pacing->bytes_budget;
    uint32_t max_burst_bytes = ngx_pacing_max_burst_size(pacing, c);
    if (pacing->last_sent_time == 0) {
        budget = max_burst_bytes;

    } else {
        budget += (now - pacing->last_sent_time) * ngx_pacing_rate_calc(pacing, c)
                    / 1000;
    }
    return ngx_min(budget, max_burst_bytes);
}

void
ngx_pacing_on_timeout(ngx_pacing_t *pacing, ngx_connection_t *c)
{
    ngx_msec_t now = ngx_current_msec;
    uint32_t budget = ngx_pacing_calc_budget(pacing, now, c);
    pacing->bytes_budget = ngx_max(budget, pacing->bytes_budget + pacing->pending_budget);
    pacing->pending_budget = 0;
    pacing->last_sent_time = now;
}

void 
ngx_pacing_on_packet_sent(ngx_pacing_t *pacing, uint32_t bytes, ngx_connection_t *c)
{
    ngx_msec_t now = ngx_current_msec;
    uint32_t budget = ngx_pacing_calc_budget(pacing, now, c);
    if (bytes > budget) {
        budget = 0;

    } else {
        budget -= bytes;
    }
    pacing->bytes_budget = budget;
    pacing->last_sent_time = now;
}

ngx_msec_t 
ngx_pacing_time_until_send(ngx_pacing_t *pacing, uint32_t bytes, ngx_connection_t *c)
{
    if (pacing->bytes_budget >= bytes) {
        return 0;
    }
    ngx_msec_t delay_ms;
    delay_ms = (uint64_t)(bytes - pacing->bytes_budget) * 1000
            / ngx_pacing_rate_calc(pacing, c);
    delay_ms = ngx_max(delay_ms, NGX_PACING_DELAY_MS);
    pacing->pending_budget = bytes - pacing->bytes_budget;
    return delay_ms;
}

void ngx_send_pacing_timer_set(ngx_pacing_t *pacing, ngx_msec_t delay, ngx_connection_t *c) {
    ngx_quic_connection_t  *qc;

    qc = ngx_quic_get_connection(c);

    if (!qc->push.timer_set) {
        ngx_add_timer(&qc->push, delay);
    }
}

int 
ngx_pacing_can_write(ngx_pacing_t *pacing, uint32_t total_bytes, ngx_connection_t *c)
{

    uint64_t delay = ngx_pacing_time_until_send(pacing, total_bytes, c);

    if (delay != 0) {
        ngx_send_pacing_timer_set(pacing, delay, c);
        return FALSE;
    }

    return TRUE;
}

void
ngx_pacing_on_app_limit(ngx_pacing_t *pacing) {
    pacing->bytes_budget = NGX_MAX_BURST_NUM;
    pacing->last_sent_time = ngx_current_msec;
}

int
ngx_pacing_is_on(ngx_pacing_t *pacing) {
    return pacing->pacing_on;
}

