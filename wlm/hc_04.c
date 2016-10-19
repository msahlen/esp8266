#include "ets_sys.h"
#include "gpio.h"
#include "osapi.h"
#include "os_type.h"

#include "hc_04.h"

// GPIOs
static const int led_pin  = 13;
static const int trig_pin = 5;
static const int echo_pin = 4;
// Constants
static const uint16 ping_trigger_length = 10;
static const float us_to_mm = (1.0 / 5.8);
// Variables
static volatile os_timer_t trigger_timer;
static uint32 time_stamp = 0;
static int trigger_interval_ms = 0;
 
// Trigger measurement
static void trigger(void *arg)
{
    // set high, wait 10 us, the set low again
    gpio_output_set((1 << trig_pin), 0, 0, 0);
    os_delay_us(ping_trigger_length);
    gpio_output_set(0, (1 << trig_pin), 0, 0);
}

// Interrupt handler
static void echo_handler(int *opaque) 
{
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);    
    
    // GPIO is hioh, start measurement
    if(GPIO_REG_READ(GPIO_IN_ADDRESS) & (1 << echo_pin))
    {
        time_stamp = system_get_time();    
    }
    // GPIO is low, calulate distance
    else
    {
        os_printf("Distance : %d mm\n", (int)((system_get_time() - time_stamp) * us_to_mm));    
    }
          
    // Reenable interrupt
    if(gpio_status & BIT(echo_pin))
    {
        gpio_pin_intr_state_set(GPIO_ID_PIN(echo_pin), GPIO_PIN_INTR_DISABLE);
        GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(echo_pin));
        gpio_pin_intr_state_set(GPIO_ID_PIN(echo_pin), GPIO_PIN_INTR_ANYEDGE);
    }
}   

void ICACHE_FLASH_ATTR init_hc_04(int interval_ms)
{   
    trigger_interval_ms = interval_ms;

    // init gpio subsystem
    gpio_init();

    // Configure GPIO pins
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);

    gpio_output_set(0, (1 << trig_pin), (1 << led_pin) | (1 << trig_pin),(1 << echo_pin));

    // Init interrupt
    ETS_GPIO_INTR_DISABLE();
    ETS_GPIO_INTR_ATTACH(echo_handler, 0);
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(echo_pin));
    gpio_pin_intr_state_set(GPIO_ID_PIN(echo_pin), GPIO_PIN_INTR_ANYEDGE);
    ETS_GPIO_INTR_ENABLE();

    // setup timer (500ms, repeating)
    os_timer_setfn(&trigger_timer, (os_timer_func_t *)trigger, NULL);
    os_timer_arm(&trigger_timer, trigger_interval_ms, 1);
}



