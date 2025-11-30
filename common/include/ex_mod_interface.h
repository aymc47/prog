
#ifndef _EX_MOD_CAN_INTERFACE_H_
#define _EX_MOD_CAN_INTERFACE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "ex_lib_queue.h"

#define TX_CHANNEL_0    0
#define TX_CHANNEL_1    1
#define TX_CHANNEL_2    2
#define TX_CHANNEL_3    3
#define TX_CHANNEL_NUM  4

extern Queue g_mod_if_can_rx_q;
extern Queue g_mod_if_can_tx_q[TX_CHANNEL_NUM];

extern int mod_interface_init(void);

#endif  // _EX_MOD_CAN_INTERFACE_H_