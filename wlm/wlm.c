#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"

static const int pin = 2;
static volatile os_timer_t blink_timer;

void timerfunc(void *arg)
{
  //Do blinky stuff
  if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & (1 << pin))
  {
    // set gpio low
    gpio_output_set(0, (1 << pin), 0, 0);
  }
  else
  {
    // set gpio high
    gpio_output_set((1 << pin), 0, 0, 0);
    os_printf("I'm high\n");
  }
}

void ICACHE_FLASH_ATTR user_init()
{
  uart_div_modify(0, UART_CLK_FREQ / 115200);      
  os_printf("SDK version:%s\n", system_get_sdk_version());
  os_printf("Module Test\nChip_id: %lu\n", system_get_chip_id());
  
  // init gpio sussytem
  gpio_init();

  // GPIO2, set as output
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2); 
  gpio_output_set(0, 0, (1 << pin), 0);

  // setup timer (500ms, repeating)
  os_timer_setfn(&blink_timer, (os_timer_func_t *)timerfunc, NULL);
  os_timer_arm(&blink_timer, 500, 1);
}
