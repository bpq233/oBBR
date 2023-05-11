#include <ngx_event_quic_connection.h>

int cnt1[100];

void init_Loss_Filter(Loss_Filter *loss_filter) {
    loss_filter->cnt = 0;
    loss_filter->rank = 0;
    for (int i = 0; i < 100; i++) {
        loss_filter->loss[i]=1;
    }
}

void insertLoss(Loss_Filter *loss_filter, uint64_t loss) {
    loss_filter->loss[loss_filter->cnt]= loss;
    loss_filter->cnt++;
    loss_filter->cnt %= 100;
}

int Loss_Rank(Loss_Filter *loss_filter, uint64_t loss){
    int c = 0;
    for (int i = 0; i < 100; i++) {
        if (loss_filter->loss[i] > loss - 1e-3) {
            c++;
        }
    }
    return c;
}