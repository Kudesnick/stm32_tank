/**
  * @file    main.c
  * @author  kudesnick (kudesnick@inbox.ru)
  * @version V0.0.1
  * @date    27-Jun-2021
  */

#include "RTE_Components.h"
#include CMSIS_device_header

#if !defined(__CC_ARM) && defined(__ARMCC_VERSION) && !defined(__OPTIMIZE__)
    /*
    Without this directive, it does not start if -o0 optimization is used and the "main"
    function without parameters.
    see http://www.keil.com/support/man/docs/armclang_mig/armclang_mig_udb1499267612612.htm
    */
    __asm(".global __ARM_use_no_argv\n\t" "__ARM_use_no_argv:\n\t");
#endif

#define GPIO_USR_LED_RCC  RCC_APB2Periph_GPIOC
#define GPIO_USR_LED_PORT GPIOC
#define GPIO_USR_LED_PIN  GPIO_Pin_13

#define PWM_TIMER_RCC   RCC_APB2Periph_TIM1
#define PWM_TIMER       TIM1
#define PWM_TIMER_IRQ   TIM1_UP_IRQn
#define GPIO_PWM_RCC    RCC_APB2Periph_GPIOA
#define GPIO_PWM_PORT   GPIOA
#define GPIO_PWM_PIN_L  GPIO_Pin_8
#define GPIO_PWM_PIN_R  GPIO_Pin_9

#define PWM_GRADE       16
#define PWM_FREQ        50 // (Hz)

#define GPIO_EN_RCC     RCC_APB2Periph_GPIOB
#define GPIO_EN_PORT    GPIOB
#define GPIO_EN_PIN_LF  GPIO_Pin_12
#define GPIO_EN_PIN_LR  GPIO_Pin_13
#define GPIO_EN_PIN_RF  GPIO_Pin_14
#define GPIO_EN_PIN_RR  GPIO_Pin_15
#define EN_PIN_MASK     (GPIO_EN_PIN_LF | GPIO_EN_PIN_LR | GPIO_EN_PIN_RF | GPIO_EN_PIN_RR)
#define EN_L_PIN_MASK   (GPIO_EN_PIN_LF | GPIO_EN_PIN_LR)
#define EN_R_PIN_MASK   (GPIO_EN_PIN_RF | GPIO_EN_PIN_RR)

#define GPIO_BTN_RCC    RCC_APB2Periph_GPIOB
#define GPIO_BTN_PORT   GPIOB
#ifndef BTN_ROT
#define GPIO_BTN_PIN_U  GPIO_Pin_5
#define GPIO_BTN_PIN_C  GPIO_Pin_6
#define GPIO_BTN_PIN_L  GPIO_Pin_7
#define GPIO_BTN_PIN_D  GPIO_Pin_8
#define GPIO_BTN_PIN_R  GPIO_Pin_9
#else
#define GPIO_BTN_PIN_U  GPIO_Pin_8
#define GPIO_BTN_PIN_C  GPIO_Pin_6
#define GPIO_BTN_PIN_L  GPIO_Pin_9
#define GPIO_BTN_PIN_D  GPIO_Pin_5
#define GPIO_BTN_PIN_R  GPIO_Pin_7
#endif
#define BTN_PIN_MASK    \
    (GPIO_BTN_PIN_U | GPIO_BTN_PIN_C | GPIO_BTN_PIN_L | GPIO_BTN_PIN_D | GPIO_BTN_PIN_R)

void TIM1_UP_IRQHandler(void)
{
    volatile uint16_t tst = GPIO_ReadInputData(GPIO_BTN_PORT);

    if ((GPIO_ReadInputData(GPIO_BTN_PORT) & BTN_PIN_MASK) == BTN_PIN_MASK)
    {
        // GPIO_LED blink
        static volatile uint32_t i;
        if (++i > (500 / 25))
        {
            uint16_t tmp = GPIO_ReadOutputData(GPIO_USR_LED_PORT);
            tmp ^= GPIO_USR_LED_PIN;
            GPIO_Write(GPIO_USR_LED_PORT, tmp);
            
            i = 0;
        }
    }
    
    TIM_ClearITPendingBit(PWM_TIMER, TIM_IT_Update);
}

static void _init(void)
{
    // GPIO_LED
    RCC_APB2PeriphClockCmd(GPIO_USR_LED_RCC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure =
    {
        .GPIO_Speed = GPIO_Speed_2MHz, 
        .GPIO_Mode  = GPIO_Mode_Out_OD,
        .GPIO_Pin   = GPIO_USR_LED_PIN,
    };
    GPIO_Init(GPIO_USR_LED_PORT, &GPIO_InitStructure);
    
    // GPIO_BTNS
    RCC_APB2PeriphClockCmd(GPIO_BTN_RCC, ENABLE);
    GPIO_InitStructure = (GPIO_InitTypeDef)
    {
        .GPIO_Speed = GPIO_Speed_2MHz, 
        .GPIO_Mode  = GPIO_Mode_IPU,
        .GPIO_Pin   = BTN_PIN_MASK,
    };
    GPIO_Init(GPIO_BTN_PORT, &GPIO_InitStructure);
    
    // PWM_PINS
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | GPIO_PWM_RCC, ENABLE);
    GPIO_InitStructure = (GPIO_InitTypeDef)
    {
        .GPIO_Speed = GPIO_Speed_2MHz, 
        .GPIO_Mode  = GPIO_Mode_AF_PP,
        .GPIO_Pin   = GPIO_PWM_PIN_L | GPIO_PWM_PIN_R,
    };
    GPIO_Init(GPIO_PWM_PORT, &GPIO_InitStructure);
    
    // GPIO_EN
    RCC_APB2PeriphClockCmd(GPIO_EN_RCC, ENABLE);
    GPIO_InitStructure = (GPIO_InitTypeDef)
    {
        .GPIO_Speed = GPIO_Speed_2MHz, 
        .GPIO_Mode  = GPIO_Mode_Out_PP,
        .GPIO_Pin   = EN_PIN_MASK,
    };
    GPIO_Init(GPIO_EN_PORT, &GPIO_InitStructure);
    uint16_t tmp = GPIO_ReadOutputData(GPIO_EN_PORT);
    tmp &= ~EN_PIN_MASK;
    GPIO_Write(GPIO_EN_PORT, tmp);
    
    // PWM_TIMER
    RCC_APB2PeriphClockCmd(PWM_TIMER_RCC, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_BaseConfig =
    {
        .TIM_Prescaler          = SystemCoreClock / (PWM_FREQ * PWM_GRADE) - 1,
        .TIM_CounterMode        = TIM_CounterMode_Up,
        .TIM_Period             = PWM_GRADE - 1,
        .TIM_ClockDivision      = 0,
        .TIM_RepetitionCounter  = 0,
    };
    TIM_TimeBaseInit(PWM_TIMER, &TIM_BaseConfig);
    
    TIM_OCInitTypeDef TIM_OCConfig =
    {
        .TIM_OCMode         = TIM_OCMode_PWM1,
        .TIM_OutputState    = TIM_OutputState_Enable,
        .TIM_OutputNState   = TIM_OutputNState_Disable,
        .TIM_Pulse          = PWM_GRADE / 2,
        .TIM_OCIdleState    = TIM_OCIdleState_Set,
        .TIM_OCPolarity     = TIM_OCPolarity_High,
    };
    TIM_OC1Init(PWM_TIMER, &TIM_OCConfig);
    TIM_OC2Init(PWM_TIMER, &TIM_OCConfig);
    
    NVIC_EnableIRQ(PWM_TIMER_IRQ);
    TIM_ITConfig(PWM_TIMER, TIM_IT_Update, ENABLE);
    TIM_Cmd(PWM_TIMER, ENABLE);
    TIM_CtrlPWMOutputs(PWM_TIMER, ENABLE);
}

static void _loop(void)
{
    __WFI();
}

int main(void)
{    
    _init();
    
    for(;;)
    {
        _loop();
    }
}
