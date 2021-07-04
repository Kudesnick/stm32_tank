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
 
bool appl_serial_handle(char *const _str, const uint8_t _max_size)
{
    while(strchr(_str, '\r'))
    {
        *strchr(_str, '\r') = '\0';
    }
    
    if (false
        || strcmp("stop", _str) == 0
        || strcmp("STOP", _str) == 0
        || strcmp("off", _str) == 0
        || strcmp("OFF", _str) == 0
       )
    {
            bsp_pwm_set(PWM_LEFT, 0);
            bsp_pwm_set(PWM_RIGHT, 0);
            strcpy(_str, "System off!\r");
    }
    else if (strcmp("info", _str) == 0)
    {
        sprintf(_str, "freq:%d, grade:%d\r", PWM_FREQ, PWM_GRADE);
    }
    else if (strcmp("state", _str) == 0)
    {
        sprintf(_str, "L:%d, R:%d\r", bsp_pwm_get(PWM_LEFT), bsp_pwm_get(PWM_RIGHT));
    }
    else
    {
        int16_t left = 0;
        int16_t right = 0;
        
        if (false
            || sscanf(_str, "L%dR%d", (int *)&left, (int *)&right) == 2
            || sscanf(_str, "R%dL%d", (int *)&left, (int *)&right) == 2
            )
        {
            bsp_pwm_set(PWM_LEFT, left);
            bsp_pwm_set(PWM_RIGHT, right);
        }
        else if (sscanf(_str, "L%d", (int *)&left) == 1)
        {
            bsp_pwm_set(PWM_LEFT, left);        
        }
        else if (sscanf(_str, "R%d", (int *)&right) == 1)
        {
            bsp_pwm_set(PWM_RIGHT, right);        
        }
        else
        {
            strcpy(_str, "Bad syntax!\r");
        }
    }

    return true;
}
