/**
 * @file bsp_btn.c
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

#include "bsp_btn.h"


#define BTN_ACTIVE      (1)

#define GPIO_BTN_RCC    RCC_APB2Periph_GPIOB
#define GPIO_BTN_PORT   GPIOB

#define BTN_PIN_MASK    \
    (GPIO_BTN_PIN_U | GPIO_BTN_PIN_C | GPIO_BTN_PIN_L | GPIO_BTN_PIN_D | GPIO_BTN_PIN_R)


void bsp_btn_init(void)
{
    RCC_APB2PeriphClockCmd(GPIO_BTN_RCC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure =
    {
        .GPIO_Speed = GPIO_Speed_2MHz, 
        .GPIO_Mode  = GPIO_Mode_IPU,
        .GPIO_Pin   = BTN_PIN_MASK,
    };
    GPIO_Init(GPIO_BTN_PORT, &GPIO_InitStructure);
}


uint16_t bsp_btn_get_port_val(void)
{
#if BTN_ACTIVE == 0
    return GPIO_ReadInputData(GPIO_BTN_PORT) & BTN_PIN_MASK;
#else
    return (GPIO_ReadInputData(GPIO_BTN_PORT) & BTN_PIN_MASK) ^ BTN_PIN_MASK;
#endif
}
