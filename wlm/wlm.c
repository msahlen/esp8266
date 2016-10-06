#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"

static const int led_pin  = 13;
static const int trig_pin = 5;
static const int echo_pin = 4;
static const uint16 ping_trigger_length = 10;
static const float us_to_mm = (1.0 / 5.8);

static volatile os_timer_t blink_timer;
uint32 time_stamp = 0;

void ICACHE_FLASH_ATTR trigger(void)
{
    float ctr = 0;    
    os_printf("Trigger\n");    
    // set high, wait 10 ms, the set low again
    gpio_output_set((1 << trig_pin), 0, 0, 0);
    os_delay_us(ping_trigger_length);
    gpio_output_set(0, (1 << trig_pin), 0, 0);
}

LOCAL void timerfunc(void *arg)
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
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);    

    if(GPIO_REG_READ(GPIO_IN_ADDRESS) & (1 << echo_pin))
    {
        time_stamp = system_get_time();    
    }
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
    ETS_GPIO_INTR_DISABLE();
    ETS_GPIO_INTR_ATTACH(echo_handler, 0);
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(echo_pin));
    gpio_pin_intr_state_set(GPIO_ID_PIN(echo_pin), GPIO_PIN_INTR_ANYEDGE);  
    ETS_GPIO_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR user_init()
{
    // Set baud rate      
    uart_div_modify(0, UART_CLK_FREQ / 115200);      
    os_printf("SDK version:%s\n", system_get_sdk_version());
  
    setup_gpios();
    setup_interrupts();
    
    // setup timer (500ms, repeating)
    os_timer_setfn(&blink_timer, (os_timer_func_t *)timerfunc, NULL);
    os_timer_arm(&blink_timer, 5000, 1);
}
