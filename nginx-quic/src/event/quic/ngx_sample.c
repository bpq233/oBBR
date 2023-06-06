#include <ngx_event_quic_connection.h>


/**
 * see https://tools.ietf.org/html/draft-cheng-iccrg-delivery-rate-estimation-00#section-3.3
 */
/* Upon receiving ACK, fill in delivery rate sample rs. */
bool
ngx_generate_sample(ngx_connection_t *c)
{
    ngx_quic_congestion_t  *cg;
    ngx_quic_connection_t  *qc;

    qc = ngx_quic_get_connection(c);
    cg = &qc->congestion;

    ngx_sample_t *sampler = &cg->sampler;

     /* Clear app-limited field if bubble is ACKed and gone. */
    if (cg->app_limited 
        && cg->delivered > cg->app_limited)
    {
        cg->app_limited = 0;
    }

    /* we do NOT have a valid sample yet. */
    if (sampler->prior_time == 0) {
        sampler->interval = 0;
        return false;
    }

    sampler->acked = cg->delivered - cg->prior_delivered;
    /* Use the longer of the send_elapsed and ack_elapsed */
    sampler->interval = ngx_max(sampler->ack_elapse, sampler->send_elapse);
    
    sampler->delivered = cg->delivered - sampler->prior_delivered;
    /* This is for BBRv2 */
    sampler->lost_pkts = cg->lost_pkts_number - sampler->prior_lost;

    if (sampler->po_sent > sampler->delivered)
        sampler->unacked = sampler->po_sent - sampler->delivered;
    else sampler->unacked = 0;

    //printf("%ld %d %.3lf\n", sampler->unacked, sampler->delivered, sampler->unacked * 100.0 / sampler->po_sent);

    /* 
     * Normally we expect interval >= MinRTT.
     * Note that rate may still be over-estimated when a spuriously
     * retransmitted skb was first (s)acked because "interval"
     * is under-estimated (up to an RTT). However, continuously
     * measuring the delivery rate during loss recovery is crucial
     * for connections suffer heavy or prolonged losses.
     */
    if (sampler->interval < qc->min_rtt) {
        sampler->interval = 0;
        return false;
    }
    if (sampler->interval != 0) {
        /* unit of interval is us */
        sampler->delivery_rate = (uint64_t)(1e3 * sampler->delivered / sampler->interval);
    }
    //printf("%d %ld %d %ld %d\n", cg->delivered, sampler->prior_delivered, sampler->delivered, sampler->interval, sampler->delivery_rate);
    sampler->now = ngx_current_msec;
    sampler->rtt = qc->latest_rtt;
    sampler->srtt = qc->avg_rtt;
    sampler->min_rtt = qc->min_rtt;
    sampler->in_flight = cg->in_flight;
    sampler->prior_inflight = cg->prior_in_flight;
    sampler->total_acked = cg->delivered;

    return true;
}

/* Update rs when packet is SACKed or ACKed. */
void 
ngx_update_sample(ngx_quic_frame_t *f, 
    ngx_connection_t *c)
{
    if (f->po_delivered_time == 0) {
        return; /* P already SACKed */
    }

    ngx_quic_congestion_t  *cg;
    ngx_quic_connection_t  *qc;

    qc = ngx_quic_get_connection(c);
    cg = &qc->congestion;

    ngx_sample_t *sampler = &cg->sampler;

    cg->delivered += f->plen;
    cg->delivered_time = ngx_current_msec;

    /* Update info using the newest packet: */
    /* if it's the ACKs from the first RTT round, we use the sample anyway */

    if ((!sampler->is_initialized)
        || (f->po_delivered > sampler->prior_delivered)) 
    {
        sampler->is_initialized = 1;
        sampler->prior_lost = f->po_lost;
        sampler->tx_in_flight = f->po_tx_in_flight;
        sampler->prior_delivered = f->po_delivered;
        sampler->prior_time = f->po_delivered_time;
        sampler->is_app_limited = f->po_is_app_limited;
        sampler->send_elapse = f->po_sent_time - 
                               f->po_first_sent_time;
        sampler->ack_elapse = cg->delivered_time - 
                              f->po_delivered_time;
        cg->first_sent_time = f->po_sent_time;
        cg->po_sent = f->po_sent;
        sampler->po_sent = f->po_sent - f->po_first_sent;
        sampler->lagest_ack_time = ngx_current_msec;
        sampler->po_sent_time = f->po_sent_time; /* always keep it updated */
    }
    /* 
     * Mark the packet as delivered once it's SACKed to
     * avoid being used again when it's cumulatively acked.
     */
    f->po_delivered_time = 0;
}

bool
ngx_sample_check_app_limited(ngx_connection_t *c, uint32_t len)
{
    ngx_quic_congestion_t  *cg;
    ngx_quic_connection_t  *qc;

    qc = ngx_quic_get_connection(c);
    cg = &qc->congestion;

    uint8_t not_cwnd_limited = 0;
    uint32_t cwnd = cg->window;
    if (cg->in_flight < cwnd) {
        /* QUIC MSS */
        not_cwnd_limited = (cwnd - cg->in_flight) >= NGX_QUIC_MSS; 
    }

    int empty = 1;
    for (int i = 0; i < NGX_QUIC_SEND_CTX_LAST; i++) {
        if (!ngx_queue_empty(&qc->send_ctx[i].sending))
            empty = 0;
    }

    if (not_cwnd_limited    /* We are not limited by CWND. */
    && len == 0
    && empty)
    {
        cg->app_limited = (cg->delivered + cg->in_flight) 
            ? (cg->delivered + cg->in_flight) : 1;
        if (cg->app_limited > 0) {
        }
        return true;
    }

    return false;
}

void 
ngx_sample_on_sent(ngx_quic_frame_t *f, ngx_connection_t *c)
{
    ngx_quic_congestion_t  *cg;
    ngx_quic_connection_t  *qc;

    qc = ngx_quic_get_connection(c);
    cg = &qc->congestion;

    if (cg->in_flight == 0) {
        cg->delivered_time = cg->first_sent_time = ngx_current_msec;
    }
    f->po_sent = cg->send;
    f->po_first_sent = cg->po_sent;
    f->po_sent_time = ngx_current_msec;
    f->po_delivered_time = cg->delivered_time;
    f->po_first_sent_time = cg->first_sent_time;
    f->po_delivered = cg->delivered;
    f->po_is_app_limited = cg->app_limited > 0 ? true : false;
    f->po_lost = cg->lost_pkts_number;
    f->po_tx_in_flight = cg->in_flight + 
                                  f->plen;
}


