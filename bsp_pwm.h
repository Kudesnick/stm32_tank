/**
 * @file bsp_pwm.h
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

typedef enum
{
    PWM_LEFT,
    PWM_RIGHT,
    PWM_CNT,
} pwm_cnt_t;

typedef void(*pwm_callback_t)(void);

void bsp_pwm_init(const uint32_t _freq, const uint16_t _grade, const uint16_t _min_duty);
void bsp_pwm_brake(void);
void bsp_pwm_set(const uint8_t _n, const int16_t _duty);
int16_t bsp_pwm_get(const uint8_t _n);
void bsp_pwm_register_callback(const pwm_callback_t _call);
