/**
  * @file    main.c
  * @author  kudesnick (kudesnick@inbox.ru)
  * @version V0.0.1
  * @date    27-Jun-2021
  */

#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdlib.h>
#include <stdbool.h>

#include "bsp_btn.h"
#include "bsp_led.h"
#include "bsp_pwm.h"
#include "btns.h"

#if !defined(__CC_ARM) && defined(__ARMCC_VERSION) && !defined(__OPTIMIZE__)
    /*
    Without this directive, it does not start if -o0 optimization is used and the "main"
    function without parameters.
    see http://www.keil.com/support/man/docs/armclang_mig/armclang_mig_udb1499267612612.htm
    */
    __asm(".global __ARM_use_no_argv\n\t" "__ARM_use_no_argv:\n\t");
#endif

#define BLINK_INTERVAL  (1000) // (ms)
#define PWM_GRADE       (16)
#define PWM_FREQ        (50)   // (Hz)

btn_t btns[] =
{
    {.pin = GPIO_BTN_PIN_U},
    {.pin = GPIO_BTN_PIN_C},
    {.pin = GPIO_BTN_PIN_L},
    {.pin = GPIO_BTN_PIN_D},
    {.pin = GPIO_BTN_PIN_R},
};


static void _time_handler(void)
{
    const uint32_t period  = 1000 / PWM_FREQ;
    const uint16_t btns_val = bsp_btn_get_port_val();

    if (btns_val)
    {
        bsp_led_handle(period);
    }
    
    for (size_t i = 0; i < (sizeof(btns) / sizeof(btns[0])); i++)
    {
        btn_handle(&btns[i], period, btns_val);
    }
}

static void _init(void)
{
    bsp_led_init(BLINK_INTERVAL);
    bsp_btn_init();
    bsp_pwm_init(PWM_FREQ, PWM_GRADE);
    bsp_pwm_register_callback(_time_handler);
}

static void _loop(void)
{
    __WFI();
}

int main(void)
{    
    _init();
    
    for(;;)
    {
        _loop();
    }
}
