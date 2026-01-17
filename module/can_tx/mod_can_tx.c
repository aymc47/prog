#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "ex_lib_queue.h"
#include "ex_mod_can_message.h"
#include "ex_mod_can_channel.h"
#include "ex_mod_can_tx.h"

Queue g_mod_if_can_tx_q[TX_CHANNEL_NUM];

int mod_can_tx_init(void) {
    int rc = 0;
    for (int i = 0; rc == 0 && i < TX_CHANNEL_NUM; i++) {
        rc = lib_queue_init(&g_mod_if_can_tx_q[i]);
    }
    return rc;
}

int mod_can_tx_main_process(void){
    int rc = 0;
    SharedCanMessage *shared_can_msg_ptr = NULL;
    Node* node_ptr = NULL;

    for(int i = 0; i < TX_CHANNEL_NUM; i++){
        node_ptr = lib_queue_pop(&g_mod_if_can_tx_q[i],&rc);
        while(!rc && node_ptr!=NULL){
            shared_can_msg_ptr = node_ptr->m_body_ptr;
            rc = mod_can_channel_send_msg(i, shared_can_msg_ptr);
            if(!rc) {
                node_ptr = lib_queue_pop(&g_mod_if_can_tx_q[i],&rc);
            }
        }
    }
    return rc;
}

int mod_can_tx_send_msg(int8_t in_channel_id,SharedCanMessage* int_shr_can_msg_ptr){
    int rc = 0;

    if(in_channel_id < 0 || in_channel_id >= TX_CHANNEL_NUM){rc = -1;}
    else {rc = lib_queue_push(&g_mod_if_can_tx_q[in_channel_id], &int_shr_can_msg_ptr->m_node);}

    return rc;
}





