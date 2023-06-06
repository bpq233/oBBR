#ifndef _NGX_BBR_H_INCLUDED_
#define _NGX_BBR_H_INCLUDED_

#include <ngx_sample.h>
#include <ngx_window_filter.h>
#include <stdbool.h>
#include <loss_filter.h>

#define TRUE 1
#define FALSE 0
#define MSEC2SEC 1000

typedef enum {
    /* Start phase quickly to fill pipe */
    BBR_STARTUP,
    /* After reaching maximum bandwidth, lower pacing rate to drain the queue*/
    BBR_DRAIN,
    /* Steady phase */
    BBR_PROBE_BW,
    /* Slow down to empty the buffer to probe real min rtt */
    BBR_PROBE_RTT,
} ngx_bbr_mode;

typedef enum {
    BBR_NOT_IN_RECOVERY=0,
    BBR_IN_RECOVERY,
} ngx_bbr_recovery_mode;

typedef enum {
    BBR_NOT_IN_CC=0,
    BBR_PROBE_CC,
    BBR_IN_CC,
    BBR_RECOVERY_CC,
} ngx_bbr_cc_mode;

typedef struct ngx_bbr_s {
    /* Current mode */
    ngx_bbr_mode           mode;
    /* Minimum rrt in the time window, in usec */
    ngx_msec_t             min_rtt;
    /* Time stamp of min_rtt */
    uint64_t               min_rtt_stamp;
    /* Time to exit PROBE_RTT */
    ngx_msec_t             probe_rtt_round_done_stamp;
    /* Maximum bandwidth byte/sec */
    ngx_win_filter_t       bandwidth;
    ngx_win_filter_t       max_rtt;
    /* Count round trips during the connection */
    uint32_t               round_cnt;
    /* Start of an measurement? */
    bool                   round_start;
    /* packet delivered value denoting the end of a packet-timed round trip */
    uint32_t               next_round_delivered;
    /* The maximum allowed number of bytes in flight */
    uint32_t               congestion_window;
    uint32_t               bw;
    uint32_t               prior_cwnd;
    /* Initial congestion window of connection */
    uint32_t               initial_congestion_window;
    /* Current pacing rate */
    uint32_t               pacing_rate;
    /* Gain currently applied to pacing rate */
    float                  pacing_gain;
    ngx_msec_t             last_cycle_start;
    /* Gain currently applied to congestion window */
    float                  cwnd_gain;
    /* If packet loss in STARTUP without bandwidth increase, exit STARTUP and
    the connection is in recovery*/
    bool                   exit_startup_on_loss;
    /* Current pacing gain cycle offset */
    uint32_t               cycle_idx;
    /* Time that the last pacing gain cycle was started */
    uint64_t               cycle_start_stamp;
    /* Indicates whether maximum bandwidth is reached in STARTUP */
    bool                   full_bandwidth_reached;
    /* Number of rounds during which there was no significant bandwidth increase */
    uint32_t               full_bandwidth_cnt;
    /* The bandwidth compared to which the increase is measured */
    uint32_t               last_bandwidth;
    /* Indicates whether a round-trip has passed since PROBE_RTT became active */
    bool                   probe_rtt_round_done;
    /* Indicates whether the most recent bandwidth sample was marked as
    app-limited. */
    bool                   last_sample_app_limited;
    /* Indicates whether any non app-limited samples have been recorded*/
    bool                   has_non_app_limited_sample;
    /* If true, use a CWND of 0.75*BDP during probe_rtt instead of 4 packets.*/
    bool                   probe_rtt_based_on_bdp;
    /**
     * If true, skip probe_rtt and update the timestamp of the existing min_rtt to
     * now if min_rtt over the last cycle is within 12.5% of the current min_rtt.
     */
    bool                   probe_rtt_skipped_if_similar_rtt;
    /* Indicates app-limited calls should be ignored as long as there's
    enough data inflight to see more bandwidth when necessary. */
    bool                   flexible_app_limited;
    bool                   probe_rtt_disabled_if_app_limited;

    uint8_t                has_srtt;
    uint8_t                idle_restart;

    uint32_t               extra_ack[2];
    ngx_msec_t             extra_ack_stamp;
    uint32_t               extra_ack_round_rtt;
    uint32_t               extra_ack_idx;
    uint32_t               epoch_ack;
    bool                   extra_ack_in_startup;
    uint32_t               extra_ack_win_len;
    uint32_t               extra_ack_win_len_in_startup;


    ngx_msec_t             last_round_trip_time;

    /* adjust cwnd in loss recovery*/
    ngx_bbr_recovery_mode  recovery_mode;
    bool                   just_enter_recovery_mode;
    bool                   just_exit_recovery_mode;
    ngx_msec_t             recovery_start_time;
    bool                   packet_conservation;
    uint32_t               expect_bw;
    bool                   enable_expect_bw;
    uint32_t               max_expect_bw;
    bool                   enable_max_expect_bw;

// loss_filter
    uint64_t  send_rtt;
    uint64_t  resend_rtt;
    Loss_Filter loss_filter;
    int loss[110];
    int rtt[1100];

    uint32_t               loss_round_cnt;

    ngx_bbr_cc_mode        cc_mode;
    ngx_msec_t             cc_start_time;
    ngx_msec_t             probe_rtt;
    ngx_msec_t             cc_rtt;
    ngx_msec_t             loss_timer;
    uint64_t               bw_down_cnt;
    uint32_t               max_down[100];

    int _rtt_[10010];
    int idx;
    int up_rtt;
} ngx_bbr_t;

void 
ngx_bbr_init(ngx_bbr_t *bbr, ngx_sample_t *sampler);

// int f_size;
// float loss_rtt;
// int rate;
// int rank;

// Loss_Filter *loss_filter;

#endif