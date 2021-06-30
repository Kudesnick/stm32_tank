/**
 * @file bsp_gpio.h
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.06.29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once


#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdint.h>


typedef struct
{
    GPIO_TypeDef  *port;
    uint16_t      pin;
} gpio_t;
