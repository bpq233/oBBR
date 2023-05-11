#include <ngx_event_quic_connection.h>


uint32_t
ngx_win_filter_get(const ngx_win_filter_t *w)
{
    return w->s[0].val;
}

uint32_t
ngx_win_filter_reset(ngx_win_filter_t *w, uint32_t t, uint32_t nval)
{
    struct ngx_win_sample nsample = {.t = t, .val = nval };
    w->s[0] = w->s[1] = w->s[2] = nsample;
    return w->s[0].val;
}



uint64_t
ngx_win_filter_get_u64(const ngx_win_filter_t *w)
{
    return w->s[0].val;
}

uint64_t
ngx_win_filter_reset_u64(ngx_win_filter_t *w, uint32_t t, uint64_t nval)
{
    struct ngx_win_sample nsample = {.t = t, .val = nval };
    w->s[0] = w->s[1] = w->s[2] = nsample;
    return w->s[0].val;
}   

/**
 * As time advances, update the 3 estimate value 
 * Check the third value was in the window on entry
 */
static uint32_t ngx_win_filter_update(ngx_win_filter_t *w, 
                                      uint32_t win,
                                      struct ngx_win_sample *nsample)
{
    uint32_t dt = nsample->t - w->s[0].t;

    if (dt > win) {
        w->s[0] = w->s[1];
        w->s[1] = w->s[2];
        w->s[2] = *nsample;
        if (nsample->t - w->s[0].t > win) {
            w->s[0] = w->s[1];
            w->s[1] = w->s[2];
            w->s[2] = *nsample;
        }

    } else if ((w->s[1].t == w->s[0].t) && dt > win/4) {
        w->s[2] = w->s[1] = *nsample;

    } else if ((w->s[2].t == w->s[1].t) && dt > win/2) {
        w->s[2] = *nsample;
    }

    return w->s[0].val;
}

uint32_t ngx_win_filter_max(ngx_win_filter_t *w, 
                            uint32_t win, 
                            uint32_t t,
                            uint32_t nval)
{
    struct ngx_win_sample nsample = {.t = t, .val = nval};

    if ((nval >= w->s[0].val) || (t - w->s[2].t > win)) {
        return ngx_win_filter_reset(w, t, nval);
    }

    if (nval >= w->s[1].val) {
        w->s[2] = w->s[1] = nsample;
    
    } else if (nval >= w->s[2].val) {
        w->s[2] = nsample;
    }

    return ngx_win_filter_update(w, win, &nsample);
}

uint32_t ngx_win_filter_min(ngx_win_filter_t *w, 
                            uint32_t win, 
                            uint32_t t,
                            uint32_t nval)
{
    struct ngx_win_sample nsample = {.t = t, .val = nval};

    if ((nval <= w->s[0].val) || (t - w->s[2].t > win)) {
        return ngx_win_filter_reset(w, t, nval);
    }

    if (nval <= w->s[1].val) {
        w->s[2] = w->s[1] = nsample;
    
    } else if (nval <= w->s[2].val) {
        w->s[2] = nsample;
    }

    return ngx_win_filter_update(w, win, &nsample);
}

static uint64_t 
ngx_win_filter_update_u64(ngx_win_filter_t *w, uint32_t win,
                          struct ngx_win_sample *nsample)
{
    uint32_t dt = nsample->t - w->s[0].t;

    if (dt > win) {
        w->s[0] = w->s[1];
        w->s[1] = w->s[2];
        w->s[2] = *nsample;
        if (nsample->t - w->s[0].t > win) {
            w->s[0] = w->s[1];
            w->s[1] = w->s[2];
            w->s[2] = *nsample;
        }

    } else if ((w->s[1].t == w->s[0].t) && dt > win/4) {
        w->s[2] = w->s[1] = *nsample;

    } else if ((w->s[2].t == w->s[1].t) && dt > win/2) {
        w->s[2] = *nsample;
    }

    return w->s[0].val;
}

uint64_t
ngx_win_filter_max_u64(ngx_win_filter_t *w, uint32_t win, 
                       uint32_t t, uint64_t nval)
{
    struct ngx_win_sample nsample = {.t = t, .val = nval};

    if ((nval >= w->s[0].val) || (t - w->s[2].t > win)) {
        return ngx_win_filter_reset_u64(w, t, nval);
    }

    if (nval >= w->s[1].val) {
        w->s[2] = w->s[1] = nsample;

    } else if (nval >= w->s[2].val) {
        w->s[2] = nsample;
    }

    return ngx_win_filter_update_u64(w, win, &nsample);
}

uint64_t
ngx_win_filter_min_u64(ngx_win_filter_t *w, uint32_t win, 
                       uint32_t t, uint64_t nval) 
{
    struct ngx_win_sample nsample = {.t = t, .val = nval};

    if ((nval <= w->s[0].val) || (t - w->s[2].t > win)) {
        return ngx_win_filter_reset_u64(w, t, nval);
    }

    if (nval <= w->s[1].val) {
        w->s[2] = w->s[1] = nsample;

    } else if (nval <= w->s[2].val) {
        w->s[2] = nsample;
    }

    return ngx_win_filter_update_u64(w, win, &nsample);
}


