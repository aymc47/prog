#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#include "ex_lib_queue.h"
#include "ex_mod_can_message.h"
#include "ex_mod_can_channel.h"

typedef struct {
    Node m_node;
    uint8_t m_mbx_id;
    SharedCanMessage* m_can_msg_ptr;
} CanMailbox;

typedef struct {
    uint8_t m_channel_id;
    CanChannelSndCbFromISR m_snd_cb_ptr;
    CanChannelRcvCbFromISR m_rcv_cb_ptr;
    Queue m_rcv_q;

    CanMailbox m_mailbox_array[MAILBOX_SIZE_MAX];
    Queue m_free_mbx_q;
} CanChannel;

static CanChannel s_can_channel_array[TX_CHANNEL_NUM];

static void interrupt_disabled(){}
static void interrupt_enabled(){}

static inline size_t min_size(size_t a, size_t b)
{
    return (a < b) ? a : b;
}

inline int can_channel_mbx_init(CanChannel* in_can_channel_ptr){
    int rc = 0;
    rc = lib_queue_init(&in_can_channel_ptr->m_free_mbx_q);
    for(int i=0; rc==0 && i<MAILBOX_SIZE_MAX;i++){
        CanMailbox* can_mbx_ptr = &in_can_channel_ptr->m_mailbox_array[i];
        can_mbx_ptr->m_mbx_id = i;
        can_mbx_ptr->m_can_msg_ptr = NULL;
        can_mbx_ptr->m_node.m_body_ptr = can_mbx_ptr;
        rc = lib_queue_push(&in_can_channel_ptr->m_free_mbx_q, &can_mbx_ptr->m_node);
    }
    return rc;
}

int mod_can_channel_init(void){
    int rc = 0;
    for(int8_t i=0;rc==0 && i<TX_CHANNEL_NUM;i++){
        CanChannel* can_channel_ptr = &s_can_channel_array[i];
        can_channel_ptr->m_channel_id = i;
        can_channel_ptr->m_snd_cb_ptr = NULL;
        can_channel_ptr->m_rcv_cb_ptr = NULL;
        rc = lib_queue_init(&can_channel_ptr->m_rcv_q);
        if(!rc) rc = can_channel_mbx_init(can_channel_ptr);
    }
    return rc;
}

int mod_can_channel_set_snd_cb_from_isr(uint8_t in_channel_id, CanChannelSndCbFromISR in_snd_cb_ptr){
    int rc = 0;
    CanChannel* can_channel_ptr = NULL;
    
    if(in_channel_id >= TX_CHANNEL_NUM){rc = -1;}
    if(!rc){
        can_channel_ptr = &s_can_channel_array[in_channel_id];
        can_channel_ptr->m_snd_cb_ptr = in_snd_cb_ptr;
    }
    return rc;
}

int mod_can_channel_set_rcv_cb_from_isr(uint8_t in_channel_id, CanChannelRcvCbFromISR in_rcv_cb_ptr){
    int rc = 0;
    CanChannel* can_channel_ptr = NULL;
    
    if(in_channel_id >= TX_CHANNEL_NUM){rc = -1;}
    if(!rc){
        can_channel_ptr = &s_can_channel_array[in_channel_id];
        can_channel_ptr->m_rcv_cb_ptr = in_rcv_cb_ptr;
    }
    return rc;
}

int mod_can_channel_send_msg(uint8_t in_channel_id, SharedCanMessage* in_can_shared_msg_ptr){
    int rc = 0;
    CanChannel* can_channel_ptr = NULL;

    if(in_channel_id >= TX_CHANNEL_NUM){rc = -1;}
    if(!rc){
        can_channel_ptr = &s_can_channel_array[in_channel_id];
        Node* node_ptr = lib_queue_pop(&can_channel_ptr->m_free_mbx_q,&rc);
        if(!rc && node_ptr==NULL){rc = -1;}
        else{
            CanMailbox* can_mbx_ptr = node_ptr->m_body_ptr;
            can_mbx_ptr->m_can_msg_ptr = in_can_shared_msg_ptr;
            printf("Sending message on channel %d", in_channel_id);
        }
    }
    return rc;
}

CanMessage* mod_can_channel_rcv_msg(uint8_t in_channel_id, int* out_rc_ptr){
    int rc = 0;
    CanChannel* can_channel_ptr = NULL;
    CanMessage* return_can_msg_ptr = NULL;  
    
    if(in_channel_id >= TX_CHANNEL_NUM){rc = -1;}
    if(!rc){
        can_channel_ptr = &s_can_channel_array[in_channel_id];
        printf("Receiving message on channel %d\n", in_channel_id);
        interrupt_disabled();
        Node* node_ptr = lib_queue_pop(&can_channel_ptr->m_rcv_q,&rc);
        interrupt_enabled();
        if(!rc && node_ptr==NULL){rc = -1;}
        else{
            return_can_msg_ptr = node_ptr->m_body_ptr;
        }
    }
    *out_rc_ptr = rc;
    return return_can_msg_ptr;
}

void rcv_interrupt_handler(uint8_t in_channel_id,uint8_t in_msg_id,uint16_t in_data_len,uint8_t* in_data_ptr)
{
    CanChannel* can_channel_ptr = NULL;
    CanMessage* can_msg_ptr = NULL;
    int rc = 0;

    if(in_channel_id >= TX_CHANNEL_NUM){rc = -1;}

    if(!rc){
        can_channel_ptr = &s_can_channel_array[in_channel_id];
        can_msg_ptr = mod_can_message_alloc(&rc);
    }
    if(!rc) {
        Node* node_ptr = &can_msg_ptr->m_node;
        size_t copy_size = 0;
        void *cpy_ptr;

        can_msg_ptr->m_ch_id = in_channel_id;
        can_msg_ptr->m_msg_id = in_msg_id;
        can_msg_ptr->m_data_len = in_data_len;
        copy_size = min_size(in_data_len,sizeof(can_msg_ptr->m_data_array));
        cpy_ptr = memcpy(can_msg_ptr->m_data_array, in_data_ptr, copy_size);
        if(cpy_ptr != can_msg_ptr->m_data_array) {rc = -1;}
        else{
            interrupt_disabled();
            rc = lib_queue_push(&can_channel_ptr->m_rcv_q, node_ptr);
            interrupt_enabled();
        }
        if(!rc){
            if(can_channel_ptr->m_rcv_cb_ptr != NULL){
                can_channel_ptr->m_rcv_cb_ptr(in_channel_id);
            }
        }
    }
}

void send_interrupt_handler(uint8_t in_channel_id, uint8_t in_mbx_id){
    int rc = 0;

    if(in_channel_id >= TX_CHANNEL_NUM){rc = -1;}
    else{
        CanChannel* can_channel_ptr = NULL;
        can_channel_ptr = &s_can_channel_array[in_channel_id];
        if(!rc){
            SharedCanMessage* shared_can_msg_ptr = NULL;
            shared_can_msg_ptr = can_channel_ptr->m_mailbox_array[in_mbx_id].m_can_msg_ptr;
            can_channel_ptr->m_snd_cb_ptr(in_channel_id, shared_can_msg_ptr);
        }
    }
}
