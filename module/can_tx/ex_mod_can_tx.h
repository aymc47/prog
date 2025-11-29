
#ifndef _EX_MOD_CAN_TX_H_
#define _EX_MOD_CAN_TX_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "ex_lib_queue.h"
#include "ex_mod_can_message.h"

extern int mod_can_tx_init(void);
extern int mod_can_tx_wakeup(void);
extern int mod_can_tx_send(int8_t in_channel_id,SharedCanMessage* int_shr_can_msg_ptr);

#endif  // #define _EX_MOD_CAN_TX_H_
