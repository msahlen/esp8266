#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"

static const int led_pin  = 13;
static const int trig_pin = 5;
static const int echo_pin = 4;
static const uint16 ping_trigger_length = 10;
static const float us_to_cm = (1.0 / 58.0);

static volatile os_timer_t blink_timer;

LOCAL void echo_handler(int *opaque);

static void trigger(void)
{
    float ctr = 0;    
    os_printf("Trigger\n");    
    // set high, wait 10 ms, the set low again
    gpio_output_set((1 << trig_pin), 0, 0, 0);
    os_delay_us(ping_trigger_length);
    gpio_output_set(0, (1 << trig_pin), 0, 0);
    
    while((GPIO_REG_READ(GPIO_IN_ADDRESS) & (1 << echo_pin)) == 0)
    {
        os_delay_us(1);
    }

    while(GPIO_REG_READ(GPIO_IN_ADDRESS) & (1 << echo_pin))
    {
        os_delay_us(1);
        ctr++;
    }

    os_printf("Distance %d cm\n",(int) (ctr * us_to_cm));    
}

void timerfunc(void *arg)
{  
    //Do blinky stuff
    if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & (1 << led_pin))
    {
      // set low
      gpio_output_set(0, (1 << led_pin), 0, 0);
    }
    else
    {
      // set high
      gpio_output_set((1 << led_pin), 0, 0, 0);
    }

    trigger();
}

LOCAL void echo_handler(int *opaque) 
{
    os_printf("Echo received\n");     
    //uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    //os_printf("status: 0x%x\n", gpio_status);
}

void ICACHE_FLASH_ATTR setup_gpios(void)
{
    // init gpio subsystem
    gpio_init();
  
    // Configure GPIO pins
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
  
    gpio_output_set(0, (1 << trig_pin), (1 << led_pin) | (1 << trig_pin),(1 << echo_pin));        
}

void ICACHE_FLASH_ATTR setup_interrupts(void)
{
    os_printf("Setup interrupts\n");    
    //gpio_intr_handler_register(echo_handler,NULL);
    //gpio_pin_intr_state_set(GPIO_ID_PIN(echo_pin), GPIO_PIN_INTR_POSEDGE);
}

void ICACHE_FLASH_ATTR user_init()
{
    // Set baud rate      
    uart_div_modify(0, UART_CLK_FREQ / 115200);      
    os_printf("SDK version:%s\n", system_get_sdk_version());
    os_printf("Module Test\nChip_id: %lu\n", system_get_chip_id());
  
    setup_gpios();
    //setup_interrupts();
    
    // setup timer (500ms, repeating)
    os_timer_setfn(&blink_timer, (os_timer_func_t *)timerfunc, NULL);
    os_timer_arm(&blink_timer, 1000, 1);
}
