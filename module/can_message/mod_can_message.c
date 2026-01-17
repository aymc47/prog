#include "ex_mod_can_message.h"

#define CAN_MSG_NUM_MAX         20
#define SHR_CAN_MSG_NUM_MAX     100

#define ARRAY_NUM(a)   (int)(sizeof(a) / sizeof((a)[0]))

static Queue s_can_msg_q;
static Queue s_shr_can_msg_q;
static CanMessage s_can_msg_array[CAN_MSG_NUM_MAX];
static SharedCanMessage s_shr_can_msg_array[SHR_CAN_MSG_NUM_MAX];

static void disable_interrupt(void){;}
static void enable_interrupt(void){;}

int mod_can_message_init(void) {
    int rc = 0;

    rc = lib_queue_init(&s_can_msg_q);
    if(!rc) rc = lib_queue_init(&s_shr_can_msg_q);

    if(!rc) {
        CanMessage* msg_ptr = &s_can_msg_array[0];
        Queue* queue_ptr = &s_can_msg_q;    
        for(int i=0; i<ARRAY_NUM(s_can_msg_array)&& rc==0; i++) {
            msg_ptr->m_node.m_body_ptr = msg_ptr;
            rc = lib_queue_push(queue_ptr,&msg_ptr->m_node);
            msg_ptr++;
        }
    }
    
    if(!rc) {
        SharedCanMessage* msg_ptr = &s_shr_can_msg_array[0];
        Queue* queue_ptr = &s_shr_can_msg_q;    
        for(int i=0; i<ARRAY_NUM(s_shr_can_msg_array)&& rc==0; i++) {
            msg_ptr->m_node.m_body_ptr = msg_ptr;
            rc = lib_queue_push(queue_ptr,&msg_ptr->m_node);
            msg_ptr++;
        }
    }

    return rc;
}

CanMessage* mod_can_message_alloc(int* out_rc_ptr) {
    Node* node_ptr = NULL;
    CanMessage* can_msg_ptr = NULL;
    int rc = 0;

    disable_interrupt();
    node_ptr = lib_queue_pop(&s_can_msg_q,&rc);
    enable_interrupt();
    
    if (!rc && node_ptr) {
        can_msg_ptr = (CanMessage*)node_ptr->m_body_ptr;
    }
    
    if (out_rc_ptr) *out_rc_ptr = rc;
    return can_msg_ptr;
}

SharedCanMessage* mod_can_message_copy(CanMessage* in_can_msg_ptr,int* out_rc_ptr) {
    Node* node_ptr = NULL;
    SharedCanMessage* shr_can_msg_ptr = NULL;
    int rc = 0;
    if (!in_can_msg_ptr||!out_rc_ptr) rc = -1;
    
    disable_interrupt();
    node_ptr = lib_queue_pop(&s_shr_can_msg_q,&rc);
    enable_interrupt();
    
    if (!rc && node_ptr) {
        shr_can_msg_ptr = (SharedCanMessage*)node_ptr->m_body_ptr;
        shr_can_msg_ptr->m_msg_ptr = in_can_msg_ptr;
        in_can_msg_ptr->m_count++;
    }
    
    if(out_rc_ptr)*out_rc_ptr = rc;
    return shr_can_msg_ptr;
}

int mod_can_shared_message_release(SharedCanMessage* in_shr_msg_ptr){
    int rc=0;
    CanMessage* can_msg_ptr;

    if(!in_shr_msg_ptr) {rc = -1;}
    else{can_msg_ptr = in_shr_msg_ptr->m_msg_ptr;} 
    
    if (!rc){
        disable_interrupt();
        rc = lib_queue_push(&s_shr_can_msg_q,&in_shr_msg_ptr->m_node);
        enable_interrupt();
    }

    if (!rc){
        disable_interrupt();
        can_msg_ptr->m_count--;
        if (can_msg_ptr->m_count <= 0){
            rc = lib_queue_push(&s_can_msg_q,&can_msg_ptr->m_node);
        }
        enable_interrupt();
    }
    return rc;
}

int mod_can_message_release(CanMessage* in_can_msg_ptr){
    int rc =0;

    if (in_can_msg_ptr == NULL) rc=-1;
    if(!rc){
        disable_interrupt();
        if (in_can_msg_ptr->m_count <= 0){
            rc = lib_queue_push(&s_can_msg_q,&in_can_msg_ptr->m_node);
        }
        enable_interrupt();
    }
    return rc;
}

void mod_can_message_print(const CanMessage *msg)
{
    uint16_t i;

    if (msg == NULL) {
        printf("CanMessage: NULL pointer\n");
        return;
    }

    printf("CanMessage {\n");
    printf("  ch_id    : %u\n",  msg->m_ch_id);
    printf("  msg_id   : 0x%08X\n", msg->m_msg_id);
    printf("  data_len : %u\n",  msg->m_data_len);

    printf("  data     :");
    for (i = 0; i < msg->m_data_len && i < MESSAGE_SIZE_MAX; i++) {
        printf(" %02X", msg->m_data_array[i]);
    }
    printf("\n");
    printf("}\n");
}