#include "osapi.h"
#include "user_interface.h"

#include "hc_04.h"

static const int hc_04_poll_interval_ms = 5000;

void ICACHE_FLASH_ATTR user_init()
{
    // Set baud rate      
    uart_div_modify(0, UART_CLK_FREQ / 115200);      
    os_printf("SDK version:%s\n", system_get_sdk_version());
 
    // Initialize hardware 
    init_hc_04(hc_04_poll_interval_ms);    
}
