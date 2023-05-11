#ifndef _LOSS_FILTER_H_INCLUDED_
#define _LOSS_FILTER_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <stdbool.h>

typedef struct {
    uint64_t loss[100];
    float loss_now;
    uint64_t rank;
    uint64_t cnt;   

}Loss_Filter;

void init_Loss_Filter(Loss_Filter *loss_filter);
void insertLoss(Loss_Filter *loss_filter, uint64_t loss);
int Loss_Rank(Loss_Filter *loss_filter, uint64_t loss);

#endif
