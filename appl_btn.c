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

static int16_t grade;
static int16_t min_duty;
static int16_t duty_left, duty_right;

static int16_t _duty_normalise(int16_t _duty)
{
    if (_duty > grade) _duty = grade;
    if (_duty < -grade) _duty = -grade;
    if (_duty > 0 && _duty < min_duty) _duty = min_duty;
    if (_duty < 0 && _duty > -min_duty) _duty = -min_duty;
    
    return _duty;
}

static void _duty_upd(const int16_t _left, const int16_t _right)
{
    duty_left = _duty_normalise(_left);
    duty_right = _duty_normalise(_right);

    bsp_pwm_set(PWM_LEFT, duty_left);
    bsp_pwm_set(PWM_RIGHT, duty_right);
}

static bool _event_ok(const btn_event_t _event)
{
    return (_event == BTN_CLICK || _event == BTN_LONG_PRESS || _event == BTN_REPEAT);
}

static void _pwm_stop(struct btn_s *const _btn, const btn_event_t _event)
{
    (void)_btn;
    if (_event != BTN_POP)
    {
        _duty_upd(0, 0);
    }
}

static void _pwm_forward(struct btn_s *const _btn, const btn_event_t _event)
{
    (void)_btn;
    if (_event_ok(_event))
    {
        const int16_t tmp = (duty_left + duty_right + 2) / 2;
        _duty_upd(tmp, tmp);
    }
}

static void _pwm_back(struct btn_s *const _btn, const btn_event_t _event)
{
    (void)_btn;
    if (_event_ok(_event))
    {
        const int16_t tmp = (duty_left + duty_right - 2) / 2;
        _duty_upd(tmp, tmp);
    }
}

static void _pwm_left(struct btn_s *const _btn, const btn_event_t _event)
{
    (void)_btn;
    if (_event_ok(_event))
    {
        _duty_upd(duty_left - 1, duty_right + 1);
    }
}

static void _pwm_right(struct btn_s *const _btn, const btn_event_t _event)
{
    (void)_btn;
    if (_event_ok(_event))
    {
        _duty_upd(duty_left + 1, duty_right - 1);
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

void appl_btn_init(const uint16_t _grade, const uint16_t _min_duty)
{
    grade = _grade;
    min_duty = _min_duty;
}

void appl_btn_handle(const uint32_t _period, const uint16_t _port_val)
{
    for (uint8_t i = 0; i < (sizeof(btns) / sizeof(btns[0])); i++)
    {
        btn_handle(&btns[i], _period, _port_val);
    }
}
