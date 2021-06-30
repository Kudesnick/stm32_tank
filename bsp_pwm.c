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

#include "bsp_pwm.h"
#include "bsp_gpio.h"


#define PWM_TIMER_RCC   RCC_APB2Periph_TIM1
#define PWM_TIMER       TIM1
#define PWM_TIMER_IRQ   TIM1_UP_IRQn

const struct
{
    uint32_t apb;
    gpio_t   forward;
    gpio_t   back;
    gpio_t   en;
} pwm[PWM_CNT] =
{
    {
        .apb = RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,
        .forward = {GPIOA, GPIO_Pin_8},
        .back    = {GPIOB, GPIO_Pin_13},
        .en      = {GPIOB, GPIO_Pin_12},
    },
    {
        .apb = RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,
        .forward = {GPIOA, GPIO_Pin_9},
        .back    = {GPIOB, GPIO_Pin_14},
        .en      = {GPIOB, GPIO_Pin_15},
    },
};


static uint32_t freq;
static int16_t grade;
static void(*callback)(void);


void TIM1_UP_IRQHandler(void)
{
    if (callback)
    {
        callback();
    }
    
    TIM_ClearITPendingBit(PWM_TIMER, TIM_IT_Update);
}


static void _pin_od_init(const gpio_t *const _gpio)
{
    GPIO_InitTypeDef GPIO_InitStructure =
    {
        .GPIO_Speed = GPIO_Speed_2MHz,
        .GPIO_Mode  = GPIO_Mode_Out_OD,
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


void bsp_pwm_init(const uint32_t _freq, const uint32_t _grade)
{
    freq = _freq;
    grade = _grade;

    // PWM_PINS
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    for (uint8_t i = 0; i < PWM_CNT; i++)
    {
        RCC_APB2PeriphClockCmd(pwm[i].apb, ENABLE);
        _pin_od_init(&pwm[i].forward);
        _pin_od_init(&pwm[i].back);
        _pin_od_init(&pwm[i].en);
    };
    
    // PWM_TIMER
    RCC_APB2PeriphClockCmd(PWM_TIMER_RCC, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_BaseConfig =
    {
        .TIM_Prescaler          = SystemCoreClock / (freq * grade) - 1,
        .TIM_CounterMode        = TIM_CounterMode_Up,
        .TIM_Period             = grade - 1,
        .TIM_ClockDivision      = 0,
        .TIM_RepetitionCounter  = 0,
    };
    TIM_TimeBaseInit(PWM_TIMER, &TIM_BaseConfig);
    
    TIM_OCInitTypeDef TIM_OCConfig =
    {
        .TIM_OCMode         = TIM_OCMode_PWM1,
        .TIM_OutputState    = TIM_OutputState_Enable,
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


void bsp_pwm_set(const uint8_t _n, const int16_t _duty)
{
    if (_n < PWM_CNT)
    {
        if (_duty == 0)
        {
            GPIO_ResetBits(pwm[_n].en.port, pwm[_n].en.pin);
            _pin_mode_set(&pwm[_n].forward, GPIO_Mode_Out_OD);
            _pin_mode_set(&pwm[_n].back   , GPIO_Mode_Out_OD);
        }
        else
        {
            int16_t duty = (_duty > grade) ? grade : _duty;

            if (duty > 0)
            {
                _pin_mode_set(&pwm[_n].back, GPIO_Mode_Out_OD);

                _pin_mode_set(&pwm[_n].forward, GPIO_Mode_AF_PP);
            }
            else
            {
                duty = grade + duty;
                _pin_mode_set(&pwm[_n].forward, GPIO_Mode_Out_OD);

                _pin_mode_set(&pwm[_n].back, GPIO_Mode_AF_PP);
            }

            if (_n == 0)
            {
                TIM_SetCompare1(PWM_TIMER, duty);
            }
            else
            {
                TIM_SetCompare2(PWM_TIMER, duty);
            }

            GPIO_SetBits(pwm[_n].en.port, pwm[_n].en.pin);
        }
    }
}


void bsp_pwm_register_callback(void(*_call)(void))
{
    callback = _call;
}
