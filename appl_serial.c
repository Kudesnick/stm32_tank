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
 
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bsp_pwm.h"
#include "main.h"

static void _print_state(char *const _str)
{
    sprintf(_str, "L:%d R:%d\r", bsp_pwm_get(PWM_LEFT), bsp_pwm_get(PWM_RIGHT));
}

bool appl_serial_handle(char *const _str, const uint8_t _max_size)
{
    while(strchr(_str, '\r'))
    {
        *strchr(_str, '\r') = '\0';
    }
    
    if (strcmp("stop", _str) == 0)
    {
            bsp_pwm_set(PWM_LEFT, 0);
            bsp_pwm_set(PWM_RIGHT, 0);
            strcpy(_str, "System off!\r");
    }
    else if (strcmp("date", _str) == 0)
    {
        sprintf(_str, __DATE__ "\r");
    }
    else if (strcmp("info", _str) == 0)
    {
        sprintf(_str, "freq:%d grade:%d\r", PWM_FREQ, PWM_GRADE);
    }
    else if (strcmp("state", _str) == 0)
    {
        _print_state(_str);
    }
    else
    {
        int left  = 0;
        int right = 0;
        int len   = 0;
        
        if (false
            || (sscanf(_str, "L%dR%d%n", &left, &right, &len) == 2 && len == strlen(_str))
            || (sscanf(_str, "R%dL%d%n", &left, &right, &len) == 2 && len == strlen(_str))
            )
        {
            bsp_pwm_set(PWM_LEFT, left);
            bsp_pwm_set(PWM_RIGHT, right);
            _print_state(_str);
        }
        else if (sscanf(_str, "L%d%n", &left, &len) == 1 && len == strlen(_str))
        {
            bsp_pwm_set(PWM_LEFT, left);
            _print_state(_str);
        }
        else if (sscanf(_str, "R%d%n", &right, &len) == 1 && len == strlen(_str))
        {
            bsp_pwm_set(PWM_RIGHT, right);
            _print_state(_str);
        }
        else
        {
            strcpy(_str, "Bad syntax!\r");
        }
    }

    return true;
}
