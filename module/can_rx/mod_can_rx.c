
#include <string.h>
#include "ex_mod_interface.h"
#include "ex_mod_can_message.h"

typedef struct {
    uint32_t m_transfer_ch_flg;
}TRANSFER_ST;

#define ID2FLAG(id) (1 << (id))
static TRANSFER_ST g_transfer_st[2048];

static inline size_t min_size(size_t a, size_t b)
{
    return (a < b) ? a : b;
}

static inline bool check_transfer_id(uint8_t in_ch_id, CanMessage *in_can_msg_ptr, int* out_rc_ptr) {
    int rc = 0;
    bool rtn = false;
    if(in_ch_id >= TX_CHANNEL_NUM || in_can_msg_ptr->m_msg_id >= 2048){rc = -1;}
    if(!rc){
        rtn = (g_transfer_st[in_can_msg_ptr->m_msg_id].m_transfer_ch_flg) & ID2FLAG(in_ch_id);
    }
    if(out_rc_ptr) *out_rc_ptr = rc;
    return rtn;
}

int mod_can_rx_init(void){
    for(int i=0; i < 2048; i++){
        g_transfer_st[i].m_transfer_ch_flg 
            = (ID2FLAG(TX_CHANNEL_0)|ID2FLAG(TX_CHANNEL_1)|ID2FLAG(TX_CHANNEL_2)|ID2FLAG(TX_CHANNEL_3));
    }
    return 0;
}

int mod_can_rx_main_process(void){
    int rc = 0;

    CanMessage *can_msg_ptr = NULL;
    Node* node_ptr = NULL;

    node_ptr = lib_queue_pop(&g_mod_if_can_rx_q,&rc);
    if(!rc && node_ptr==NULL){rc = -1;}
    else{
        can_msg_ptr = node_ptr->m_body_ptr;
        if(can_msg_ptr == NULL) rc = -1;
    }

    for(int i = 0; rc == 0 && i < TX_CHANNEL_NUM; i++){
        if(check_transfer_id(i, can_msg_ptr, &rc)){
            SharedCanMessage* shared_can_msg_ptr = mod_can_message_copy(can_msg_ptr, &rc);
            if(!shared_can_msg_ptr && rc == 0){
                rc = lib_queue_push(&g_mod_if_can_tx_q[i], &shared_can_msg_ptr->m_node);
            }
        }
    }

    if(!rc) rc = mod_can_message_release(can_msg_ptr);
    return rc;
}

int mod_can_rx_notify(uint8_t in_ch_id,uint8_t in_msg_id,uint16_t in_data_len,uint8_t* in_data_ptr)
{
    int rc = 0;
    CanMessage *can_msg_ptr = NULL;

    can_msg_ptr = mod_can_message_alloc(&rc);
    if(!rc && can_msg_ptr==NULL)rc = -1;
    if(!rc) {
        size_t copy_size = 0;
        void *cpy_ptr;
        can_msg_ptr->m_ch_id = in_ch_id;
        can_msg_ptr->m_msg_id = in_msg_id;
        can_msg_ptr->m_data_len = in_data_len;
        copy_size = min_size(in_data_len,sizeof(can_msg_ptr->m_data_array));
        cpy_ptr = memcpy(can_msg_ptr->m_data_array, in_data_ptr, copy_size);
        if(cpy_ptr != can_msg_ptr->m_data_array) rc = -1;
    }
    if(!rc) rc = lib_queue_push(&g_mod_if_can_rx_q, &can_msg_ptr->m_node);

    return rc;
}

