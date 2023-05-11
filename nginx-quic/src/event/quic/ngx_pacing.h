#ifndef _NGX_PACING_H_INCLUDED_
#define _NGX_PACING_H_INCLUDED_

#include <ngx_config.h>
#include <ngx_core.h>
#include <stdbool.h>

typedef struct ngx_pacing_s {
    int             pacing_on;
    uint32_t        bytes_budget;
    ngx_msec_t      last_sent_time;
    uint32_t        pending_budget;
    
} ngx_pacing_t;

int ngx_pacing_is_on(ngx_pacing_t *pacing);

void ngx_pacing_init(ngx_pacing_t *pacing, int pacing_on, ngx_connection_t *c);

void ngx_pacing_on_timeout(ngx_pacing_t *pacing, ngx_connection_t *c);

void ngx_pacing_on_packet_sent(ngx_pacing_t *pacing, uint32_t bytes, ngx_connection_t *c);

void ngx_pacing_on_app_limit(ngx_pacing_t *pacing);

int ngx_pacing_can_write(ngx_pacing_t *pacing, uint32_t total_bytes, ngx_connection_t *c);

#endif /* _NGX_PACING_H_INCLUDED_ */