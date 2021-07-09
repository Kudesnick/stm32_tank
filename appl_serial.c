/**
 * @file appl_serial.c
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.07.04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bsp_pwm.h"
#include "main.h"

static void _add_dig(char *const _str, int16_t _val)
{
    uint8_t ptr = strlen(_str);
    
    if (_val < 0)
    {
        _str[ptr++] = '-';
        _val = abs(_val);
    }
    
    uint8_t dig_num = (_val == 0) ? 1 : 0;
    for (uint16_t tmp = _val; tmp > 0; tmp /= 10, dig_num++);
    
    ptr += dig_num;
    _str[ptr--] = '\0';
    
    for (uint16_t tmp = _val; dig_num > 0; tmp /= 10, dig_num--)
    {
        _str[ptr--] = tmp % 10 + '0';
        
    }
}

static void _dig_decode(char **_str, int16_t *_val)
{
    int16_t val = 0;
    bool neg = false;
    
    if (**_str == '-')
    {
        neg = true;
        (*_str)++;
    }
    
    if (**_str < '0' || **_str > '9') return;
    
    for (;**_str >= '0' && **_str <= '9'; (*_str)++)
    {
        val = val * 10 + **_str - '0';
    }
    
    *_val = (neg) ? -val : val;
}

static void _lr_decode(char *_str, int16_t *_left, int16_t *_right)
{
    int16_t left = *_left, right = *_right;

    while(*_str != '\0')
    {
        switch (*_str)
        {
            case (char)'L': _str++; _dig_decode(&_str, &left ); break;
            case (char)'R': _str++; _dig_decode(&_str, &right); break;
            default: return;
        }
    }
    
    *_left = left;
    *_right = right;
}

static void _print_state(char *const _str)
{
    strcpy(_str, "L:");
    _add_dig(_str, bsp_pwm_get(PWM_LEFT));
    strcat(_str, "R:");
    _add_dig(_str, bsp_pwm_get(PWM_RIGHT));
    strcat(_str, "\r");
}

bool appl_serial_handle(char *const _str, const uint8_t _max_size)
{
    while(strchr(_str, '\r'))
    {
        *strchr(_str, '\r') = '\0';
    }
    
    if (strcmp("brake", _str) == 0)
    {
            bsp_pwm_brake();
            strcpy(_str, "Brake!\r");
    }
    else if (strcmp("stop", _str) == 0)
    {
            bsp_pwm_set(PWM_LEFT, 0);
            bsp_pwm_set(PWM_RIGHT, 0);
            strcpy(_str, "System off!\r");
    }
    else if (strcmp("date", _str) == 0)
    {
        strcpy(_str, __DATE__ "[" __TIME__ "]" "\r");
    }
    else if (strcmp("info", _str) == 0)
    {
        strcpy(_str, "freq:");
        _add_dig(_str, PWM_FREQ);
        strcat(_str, " grade:");
        _add_dig(_str, PWM_GRADE);
        strcat(_str, "\r");
    }
    else if (strcmp("state", _str) == 0)
    {
        _print_state(_str);
    }
    else
    {
        int16_t left  = bsp_pwm_get(PWM_LEFT);
        int16_t right = bsp_pwm_get(PWM_RIGHT);

        _lr_decode(_str, &left, &right);
        
        bsp_pwm_set(PWM_LEFT, left);
        bsp_pwm_set(PWM_RIGHT, right);
        
        _print_state(_str);
    }

    return true;
}
