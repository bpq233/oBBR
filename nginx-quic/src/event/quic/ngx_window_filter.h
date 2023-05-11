#ifndef _NGX_WIN_FILTER_H_INCLUDED_
#define _NGX_WIN_FILTER_H_INCLUDED_

#include <ngx_config.h>
#include <ngx_core.h>

struct ngx_win_sample {
    ngx_msec_t      t;
    uint64_t        val;
};

typedef struct {
    struct ngx_win_sample s[3];
} ngx_win_filter_t;

uint32_t ngx_win_filter_max(ngx_win_filter_t *w,
                            uint32_t win,
                            uint32_t t,
                            uint32_t nval);

uint32_t ngx_win_filter_min(ngx_win_filter_t *w,
                            uint32_t win,
                            uint32_t t,
                            uint32_t nval);

uint64_t ngx_win_filter_max_u64(ngx_win_filter_t *w,
                                uint32_t win,
                                uint32_t t,
                                uint64_t nval);

uint64_t ngx_win_filter_min_u64(ngx_win_filter_t *w,
                                uint32_t win,
                                uint32_t t,
                                uint64_t nval);

uint32_t
ngx_win_filter_get(const ngx_win_filter_t *w);
uint32_t
ngx_win_filter_reset(ngx_win_filter_t *w, uint32_t t, uint32_t nval);
uint64_t
ngx_win_filter_get_u64(const ngx_win_filter_t *w);
uint64_t
ngx_win_filter_reset_u64(ngx_win_filter_t *w, uint32_t t, uint64_t nval);   

#endif