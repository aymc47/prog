#include <stdio.h>
#include "ex_mod_can_message.h"
#include "ex_mod_can_channel.h"
#include "ex_mod_can_rx.h"
#include "ex_mod_can_tx.h"

int main(){
    int rc = 0;
    rc = mod_can_message_init();
    if(!rc) rc = mod_can_channel_init();
    if(!rc) rc = mod_can_rx_init();
    if(!rc) rc = mod_can_tx_init();

    if(!rc){rcv_interrupt_handler(0, 0x100, 8, (uint8_t[]){0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08});}
    if(!rc) rc = mod_can_rx_main_process();
    if(!rc) rc = mod_can_tx_main_process();

    if(rc){printf("Initialization failed with code: %d\n", rc);}

    return rc;
}