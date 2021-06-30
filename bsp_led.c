/**
 * @file bsp_led.c
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.06.29
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#include "RTE_Components.h"
#include CMSIS_device_header

#include "bsp_led.h"


#define GPIO_LED_RCC  RCC_APB2Periph_GPIOC
#define GPIO_LED_PORT GPIOC
#define GPIO_LED_PIN  GPIO_Pin_13


static uint32_t blink_time;


void bsp_led_init(const uint32_t _blink_interval)
{
    blink_time = _blink_interval >> 1;

    RCC_APB2PeriphClockCmd(GPIO_LED_RCC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure =
    {
        .GPIO_Speed = GPIO_Speed_2MHz, 
        .GPIO_Mode  = GPIO_Mode_Out_OD,
        .GPIO_Pin   = GPIO_LED_PIN,
    };
    GPIO_Init(GPIO_LED_PORT, &GPIO_InitStructure);
}


void bsp_led_handle(const uint32_t _period)
{
    static uint32_t time = 0;
    time += _period;
    
    if (time >= blink_time)
    {
        uint16_t tmp = GPIO_ReadOutputData(GPIO_LED_PORT);
        tmp ^= GPIO_LED_PIN;
        GPIO_Write(GPIO_LED_PORT, tmp);
        
        time -= blink_time;
    }
}
