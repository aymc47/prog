
#include "ex_mod_interface.h"

Queue g_mod_if_can_rx_q;
Queue g_mod_if_can_tx_q[TX_CHANNEL_NUM];

extern int lib_queue_init(Queue* in_ins_ptr);

int mod_interface_init(void){
    int rc = 0;
    rc = lib_queue_init(&g_mod_if_can_rx_q);
    for (int i = 0; i < TX_CHANNEL_NUM && rc == 0; i++) {
            rc = lib_queue_init(&g_mod_if_can_tx_q[i]);
    }
    return rc;
}
