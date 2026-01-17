
#include <string.h>
#include "ex_mod_can_message.h"
#include "ex_mod_can_channel.h"
#include "ex_mod_can_rx.h"
#include "ex_mod_can_tx.h"
typedef struct {
    uint32_t m_transfer_ch_flg;
}TRANSFER_ST;

#define ID2FLAG(id) (1 << (id))
static TRANSFER_ST g_transfer_st[2048];
Queue g_mod_if_can_rx_q;

static inline bool check_transfer_id(uint8_t in_ch_id, CanMessage *in_can_msg_ptr, int* out_rc_ptr) {
    int rc = 0;
    bool rtn = false;
    if(in_ch_id >= TX_CHANNEL_NUM || in_can_msg_ptr->m_msg_id >= 2048){rc = -1;}
    if(!rc){
        if((g_transfer_st[in_can_msg_ptr->m_msg_id].m_transfer_ch_flg) & ID2FLAG(in_ch_id)) {
            rtn = true;
        }
    }
    if(out_rc_ptr) *out_rc_ptr = rc;
    return rtn;
}

int mod_can_rx_init(void){
    int rc = 0;
    for(int i=0; i < 2048; i++){
        g_transfer_st[i].m_transfer_ch_flg = 0;
    }
    g_transfer_st[0x100].m_transfer_ch_flg  = (ID2FLAG(TX_CHANNEL_0)|ID2FLAG(TX_CHANNEL_2)|ID2FLAG(TX_CHANNEL_3));

    rc = lib_queue_init(&g_mod_if_can_rx_q);
    return rc;
}

int mod_can_rx_main_process(void){
    int rc = 0;

    CanMessage *can_msg_ptr = NULL;


    /* Receive messages from all channels and push them into the queue */
    for(int i=0; rc == 0 && i < TX_CHANNEL_NUM; i++){
        can_msg_ptr = mod_can_channel_rcv_msg(i, &rc);
        while(!rc && can_msg_ptr!=NULL){
            rc = lib_queue_push(&g_mod_if_can_rx_q, &can_msg_ptr->m_node);
            if(!rc) can_msg_ptr = mod_can_channel_rcv_msg(i, &rc);
        }
    }
    
    /* Process messages in the queue and forward them to appropriate channels */
    if(!rc) {
        Node* node_ptr = lib_queue_pop(&g_mod_if_can_rx_q,&rc);
    while(!rc && node_ptr != NULL){
        can_msg_ptr = node_ptr->m_body_ptr;
        if(can_msg_ptr == NULL) rc = -1;    

        for(int i = 0; !rc && i < TX_CHANNEL_NUM; i++){
            if(check_transfer_id(i, can_msg_ptr, &rc) && !rc){
                SharedCanMessage* shared_can_msg_ptr = mod_can_message_copy(can_msg_ptr, &rc);
                    if(!rc && shared_can_msg_ptr != NULL){
                    rc = mod_can_tx_send_msg(i, shared_can_msg_ptr);
                }
            }
        }
        if(!rc) rc = mod_can_message_release(can_msg_ptr);
        if(!rc) node_ptr = lib_queue_pop(&g_mod_if_can_rx_q,&rc);
    }
    }

    return rc;
}
