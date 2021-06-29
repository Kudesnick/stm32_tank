/**
 * @file bsp_led.h
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.06.29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once


#include <stdint.h>


void bsp_led_init(const uint32_t _blink_interval);
void bsp_led_handle(const uint32_t _period);
