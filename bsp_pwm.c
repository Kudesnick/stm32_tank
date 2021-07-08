/**
 * @file bsp_pwm.c
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

#include "stdlib.h"

#include "bsp_pwm.h"
#include "bsp_gpio.h"


#define PWM_TIMER_RCC   RCC_APB2Periph_TIM1
#define PWM_TIMER       TIM1
#define PWM_TIMER_IRQ   TIM1_UP_IRQn

const struct
{
    uint32_t apb;
    gpio_t   frwd;
    gpio_t   back;
    gpio_t   pwm;
} pwm[PWM_CNT] =
{
    {
        .apb  = /* RCC_APB2Periph_GPIOA | */ RCC_APB2Periph_GPIOB,
        .frwd = {GPIOB, GPIO_Pin_12},
        .back = {GPIOB, GPIO_Pin_15},
        .pwm  = {GPIOB, GPIO_Pin_13},
    },
    {
        .apb  = RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,
        .frwd = {GPIOA, GPIO_Pin_8},
        .back = {GPIOA, GPIO_Pin_15},
        .pwm  = {GPIOB, GPIO_Pin_14},
    },
};


static uint32_t freq;
static int16_t grade;
static uint16_t min_duty;
static pwm_callback_t callback;

int16_t duty[PWM_CNT];

void TIM1_UP_IRQHandler(void)
{
    if (callback)
    {
        callback();
    }
    
    TIM_ClearITPendingBit(PWM_TIMER, TIM_IT_Update);
}


static void _pin_out_init(const gpio_t *const _gpio)
{
    GPIO_InitTypeDef GPIO_InitStructure =
    {
        .GPIO_Speed = GPIO_Speed_2MHz,
        .GPIO_Mode  = GPIO_Mode_Out_PP,
        .GPIO_Pin   = _gpio->pin,
    };
    GPIO_Init(_gpio->port, &GPIO_InitStructure);
    GPIO_ResetBits(_gpio->port, _gpio->pin);
}


static void _pin_mode_set(const gpio_t *const _gpio, const GPIOMode_TypeDef _mode)
{
    GPIO_InitTypeDef GPIO_InitStructure =
    {
        .GPIO_Speed = GPIO_Speed_2MHz,
        .GPIO_Mode  = _mode,
        .GPIO_Pin   = _gpio->pin,
    };
    GPIO_Init(_gpio->port, &GPIO_InitStructure);
}


void bsp_pwm_init(const uint32_t _freq, const uint16_t _grade, const uint16_t _min_duty)
{
    freq = _freq;
    grade = _grade;
    min_duty = (_min_duty) ? _min_duty - 1 : 0;

    // PWM_PINS
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    for (uint8_t i = 0; i < PWM_CNT; i++)
    {
        RCC_APB2PeriphClockCmd(pwm[i].apb, ENABLE);
        _pin_out_init(&pwm[i].frwd);
        _pin_out_init(&pwm[i].back);
        _pin_out_init(&pwm[i].pwm);
    };
    
    // PWM_TIMER
    RCC_APB2PeriphClockCmd(PWM_TIMER_RCC, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_BaseConfig =
    {
        .TIM_Prescaler          = SystemCoreClock / (freq * (grade + min_duty)) - 1,
        .TIM_CounterMode        = TIM_CounterMode_Up,
        .TIM_Period             = grade + min_duty - 1,
        .TIM_ClockDivision      = 0,
        .TIM_RepetitionCounter  = 0,
    };
    TIM_TimeBaseInit(PWM_TIMER, &TIM_BaseConfig);
    
    TIM_OCInitTypeDef TIM_OCConfig =
    {
        .TIM_OCMode         = TIM_OCMode_PWM1,
        .TIM_OutputState    = TIM_OutputState_Disable,
        .TIM_OutputNState   = TIM_OutputNState_Enable,
        .TIM_Pulse          = 0,
        .TIM_OCIdleState    = TIM_OCIdleState_Set,
        .TIM_OCNIdleState   = TIM_OCIdleState_Set,
        .TIM_OCPolarity     = TIM_OCPolarity_High,
        .TIM_OCNPolarity    = TIM_OCPolarity_High,
    };
    TIM_OC1Init(PWM_TIMER, &TIM_OCConfig);
    TIM_OC2Init(PWM_TIMER, &TIM_OCConfig);
    
    NVIC_EnableIRQ(PWM_TIMER_IRQ);
    TIM_ITConfig(PWM_TIMER, TIM_IT_Update, ENABLE);
    TIM_Cmd(PWM_TIMER, ENABLE);
    TIM_CtrlPWMOutputs(PWM_TIMER, ENABLE);
}


void bsp_pwm_brake(void)
{
    

    for (uint8_t i = 0; i < PWM_CNT; i++)
    {
        bsp_pwm_set(i, 0);
        GPIO_SetBits(pwm[i].pwm.port, pwm[i].pwm.pin);
    }
}


void bsp_pwm_set(const uint8_t _n, const int16_t _duty)
{
    if (_n < PWM_CNT)
    {
        if (_duty == 0)
        {
            duty[_n] = 0;
            _pin_out_init(&pwm[_n].pwm);
            GPIO_ResetBits(pwm[_n].frwd.port, pwm[_n].frwd.pin);
            GPIO_ResetBits(pwm[_n].back.port, pwm[_n].back.pin);
        }
        else
        {
            duty[_n] = (_duty > grade) ? grade : _duty;
            duty[_n] = (duty[_n] < (-grade)) ? (-grade) : duty[_n];
            
            const uint16_t tmp = abs(duty[_n]) + min_duty;

            const gpio_t *const set_pin   = (duty[_n] > 0) ? &pwm[_n].frwd : &pwm[_n].back;
            const gpio_t *const reset_pin = (duty[_n] < 0) ? &pwm[_n].frwd : &pwm[_n].back;

            GPIO_ResetBits(reset_pin->port, reset_pin->pin);
            GPIO_SetBits  (  set_pin->port,   set_pin->pin);

            if (_n == PWM_LEFT)
            {
                TIM_SetCompare1(PWM_TIMER, tmp);
            }
            else if (_n == PWM_RIGHT)
            {
                TIM_SetCompare2(PWM_TIMER, tmp);
            }

            _pin_mode_set(&pwm[_n].pwm, GPIO_Mode_AF_PP);
        }
    }
}

int16_t bsp_pwm_get(const uint8_t _n)
{
    return (_n < PWM_CNT) ? duty[_n] : 0;
}

void bsp_pwm_register_callback(const pwm_callback_t _call)
{
    callback = _call;
}
