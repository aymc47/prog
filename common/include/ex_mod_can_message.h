
#ifndef _EX_MOD_CAN_MESSAGE_H_
#define _EX_MOD_CAN_MESSAGE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "ex_lib_queue.h"
#define MESSAGE_SIZE_MAX 64

typedef struct {
    Node m_node;
    uint32_t m_count;
    uint8_t m_ch_id;                        /*can channel id*/
    uint32_t m_msg_id;                       /*can message id*/
    uint16_t m_data_len;                    /*can message len*/
    uint8_t m_data_array[MESSAGE_SIZE_MAX]; /*can message data*/
} CanMessage;

typedef struct {
    Node m_node;
    CanMessage* m_msg_ptr;
} SharedCanMessage;

extern int mod_can_message_init(void);
extern CanMessage* mod_can_message_alloc(int* out_rc_ptr);  /*interrupt ok*/
extern SharedCanMessage* mod_can_message_copy(CanMessage* in_can_msg_ptr,int* out_rc_ptr);
extern int mod_can_message_release(CanMessage* in_shr_can_msg_ptr);
extern int mod_can_shared_message_release(SharedCanMessage* in_shr_can_msg_ptr);

#endif  // _EX_MOD_CAN_MESSAGE_H_