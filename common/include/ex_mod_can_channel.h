
#ifndef _EX_MOD_CAN_CHANNEL_H_
#define _EX_MOD_CAN_CHANNEL_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "ex_lib_queue.h"
#include "ex_mod_can_message.h"

#define TX_CHANNEL_0    0
#define TX_CHANNEL_1    1
#define TX_CHANNEL_2    2
#define TX_CHANNEL_3    3
#define TX_CHANNEL_NUM  4

#define MAILBOX_SIZE_MAX 64 /*max size of mailbox for each mail box*/ 

typedef void (*CanChannelSndCbFromISR)(uint8_t m_channel_id, SharedCanMessage* m_shared_can_msg_ptr);
typedef void (*CanChannelRcvCbFromISR)(uint8_t m_channel_id);

extern int mod_can_channel_init(void);
extern int mod_can_channel_set_snd_cb_from_isr(uint8_t in_channel_id, CanChannelSndCbFromISR in_snd_cb_ptr);
extern int mod_can_channel_set_rcv_cb_from_isr(uint8_t in_channel_id, CanChannelRcvCbFromISR in_rcv_cb_ptr);
extern int mod_can_channel_send_msg(uint8_t in_channel_id, SharedCanMessage* in_can_msg_ptr);
extern CanMessage* mod_can_channel_rcv_msg(uint8_t in_channel_id, int* out_rc_ptr);

#endif  // _EX_MOD_CAN_CHANNEL_H_