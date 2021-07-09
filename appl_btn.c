/**
 * @file appl_btn.c
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.06.30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "RTE_Components.h"
#include CMSIS_device_header

#include "appl_btn.h"
#include "btns.h"
#include "bsp_btn.h"
#include "bsp_pwm.h"

#define INC (16)
#define MAX (512)

static void _duty_upd(const int16_t _left, const int16_t _right)
{
    bsp_pwm_set(PWM_LEFT, _left);
    bsp_pwm_set(PWM_RIGHT, _right);
}

__INLINE static bool _event_ok(const btn_event_t _event)
{
    return (_event == BTN_PUSH || _event == BTN_LONG_PRESS || _event == BTN_REPEAT);
}

static void _pwm_stop(struct btn_s *const _btn, const btn_event_t _event)
{
    (void)_btn;
    static bool not_brake = false;
    switch (_event)
    {
        case BTN_PUSH:
            _duty_upd(0, 0);
            break;
    
        case BTN_POP:
            if (!not_brake)
            {
                bsp_pwm_brake();
            }
            
            not_brake = false;
            break;
        
        case BTN_LONG_PRESS:
            not_brake = true;
            break;
        
        default:
            break;
    }
}

static void _pwm_forward(struct btn_s *const _btn, const btn_event_t _event)
{
    (void)_btn;
    if (_event_ok(_event))
    {
        const int16_t tmp = (bsp_pwm_get(PWM_LEFT) + bsp_pwm_get(PWM_RIGHT) + INC * 2) / 2;
        _duty_upd(tmp, tmp);
    }
    if (_event == BTN_DBL_CLICK)
    {
        _duty_upd(MAX, MAX);
    }
}

static void _pwm_back(struct btn_s *const _btn, const btn_event_t _event)
{
    (void)_btn;
    if (_event_ok(_event))
    {
        const int16_t tmp = (bsp_pwm_get(PWM_LEFT) + bsp_pwm_get(PWM_RIGHT) - INC * 2) / 2;
        _duty_upd(tmp, tmp);
    }
    if (_event == BTN_DBL_CLICK)
    {
        _duty_upd(-MAX, -MAX);
    }
}

static void _pwm_left(struct btn_s *const _btn, const btn_event_t _event)
{
    (void)_btn;
    if (_event_ok(_event))
    {
        _duty_upd(bsp_pwm_get(PWM_LEFT) - INC, bsp_pwm_get(PWM_RIGHT) + INC);
    }
}

static void _pwm_right(struct btn_s *const _btn, const btn_event_t _event)
{
    (void)_btn;
    if (_event_ok(_event))
    {
        _duty_upd(bsp_pwm_get(PWM_LEFT) + INC, bsp_pwm_get(PWM_RIGHT) - INC);
    }
}

btn_t btns[5] =
{
    {.pin = GPIO_BTN_PIN_U, .event = _pwm_forward},
    {.pin = GPIO_BTN_PIN_C, .event = _pwm_stop   },
    {.pin = GPIO_BTN_PIN_L, .event = _pwm_left   },
    {.pin = GPIO_BTN_PIN_D, .event = _pwm_back   },
    {.pin = GPIO_BTN_PIN_R, .event = _pwm_right  },
};

void appl_btn_handle(const uint32_t _period, const uint16_t _port_val)
{
    for (uint8_t i = 0; i < (sizeof(btns) / sizeof(btns[0])); i++)
    {
        btn_handle(&btns[i], _period, _port_val);
    }
}
