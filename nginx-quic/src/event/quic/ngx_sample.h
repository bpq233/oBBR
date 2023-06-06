#ifndef _NGX_SAMPLE_H_INCLUDED_
#define _NGX_SAMPLE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <stdbool.h>


typedef struct ngx_sample_s {
    /* sampling time */
    ngx_msec_t       now;
    /* the number of packets that have been transferred when the packet currently in ack is being sent */
    uint64_t         prior_delivered;
    /* time interval between samples */
    ngx_msec_t       interval;
    /* the amount of data transferred (ack) between two samples */
    uint32_t         delivered;
    /* the amount of newly delivered data*/
    uint32_t         acked;
    /* the amount of data sent but not received ack */
    uint32_t         in_flight;
    /* before processing this ack */
    uint32_t         prior_inflight;
    /* sampled rtt */
    ngx_msec_t       rtt;
    uint32_t         is_app_limited;
    /* whether packet loss */
    uint32_t         loss;
    uint64_t         total_acked;
    uint64_t         total_loss;
    ngx_msec_t       srtt;
    ngx_msec_t       min_rtt;
    /* used to determine if generate_sample needs to be called */
    ngx_msec_t       prior_time;
    ngx_msec_t       ack_elapse;
    ngx_msec_t       send_elapse;
    uint32_t         delivery_rate;
    ngx_msec_t       lagest_ack_time;
 
    ngx_msec_t       po_sent_time;

    bool       is_initialized;
 
    /* for BBRv2 */ 
    uint32_t         prior_lost;
    uint64_t         tx_in_flight;
    uint32_t         lost_pkts;

    uint64_t         po_sent;
    uint64_t         unacked;     

} ngx_sample_t;

// bool ngx_generate_sample(ngx_connection_t *c);
// void ngx_update_sample(ngx_quic_frame_t *f, ngx_connection_t *c);
// bool ngx_sample_check_app_limited(ngx_connection_t *c, uint32_t len);
// void ngx_sample_on_sent(ngx_quic_frame_t *f, ngx_connection_t *c);

#endif