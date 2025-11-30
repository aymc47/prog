
#ifndef _EX_MOD_CAN_RX_H_
#define _EX_MOD_CAN_RX_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

extern int mod_can_rx_init(void);
extern int mod_can_rx_main_process(void);
extern int mod_can_rx_notify(uint8_t in_ch_id,uint8_t in_msg_id,uint16_t in_data_len,uint8_t* in_data_ptr);
#endif  // _EX_MOD_CAN_RX_H_