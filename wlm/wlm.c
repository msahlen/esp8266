#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"

static const int led_pin  = 2;
static const int trig_pin = 12;
static const int echo_pin = 13;
static volatile os_timer_t blink_timer;

void timerfunc(void *arg)
{
  //Do blinky stuff
  if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & (1 << led_pin))
  {
    // set gpio low
    gpio_output_set(0, (1 << led_pin), 0, 0);
  }
  else
  {
    // set gpio high
    gpio_output_set((1 << led_pin), 0, 0, 0);
  }
}

void setup_gpios(void)
{
  // init gpio subsystem
  gpio_init();
  
  // Configure GPIO pins
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
  
  gpio_output_set(0, 0, (1 << led_pin) | (1 << trig_pin),(1 << echo_pin));        
}

void ICACHE_FLASH_ATTR user_init()
{
  // Set baud rate      
  uart_div_modify(0, UART_CLK_FREQ / 115200);      
  os_printf("SDK version:%s\n", system_get_sdk_version());
  os_printf("Module Test\nChip_id: %lu\n", system_get_chip_id());
  
  setup_gpios();

  // setup timer (500ms, repeating)
  os_timer_setfn(&blink_timer, (os_timer_func_t *)timerfunc, NULL);
  os_timer_arm(&blink_timer, 500, 1);
}
