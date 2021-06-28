/**
  * @file    main.c
  * @author  kudesnick (kudesnick@inbox.ru)
  * @version V0.0.1
  * @date    27-Jun-2021
  */

#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdlib.h>
#include <stdbool.h>

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

#define BTN_ACTIVE          (0)
#define BTN_TOUT_DEB        (40)
#define BTN_DBL_TOUT        (400)
#define BTN_LONG_TOUT       (500)
#define BTN_REPEAT_TOUT     (1500)
#define BTN_REPEAT_INTERVAL (500)

typedef enum
{
    BTN_NA = 0,
    BTN_PUSH,
    BTN_POP,
    BTN_CLICK,
    BTN_LONG_PRESS,
    BTN_REPEAT,
    BTN_DBL = 0x10,
    BTN_DBL_CLICK      = BTN_CLICK      | BTN_DBL,
    BTN_DBL_LONG_PRESS = BTN_LONG_PRESS | BTN_DBL,
    BTN_DBL_REPEAT     = BTN_REPEAT     | BTN_DBL,
} btn_event_t;

typedef struct btn_s
{
    const uint16_t pin        ;
    uint32_t       timer      ; // universal timer
    uint32_t       push_timer ; // interval between events BTN_PUSH
    btn_event_t    prev_status;
    btn_event_t    dbl        ;
    void           (*event)(struct btn_s *const _btn, const btn_event_t _event);
} btn_t;

btn_t btns[] =
{
    {.pin = GPIO_BTN_PIN_U},
    {.pin = GPIO_BTN_PIN_C},
    {.pin = GPIO_BTN_PIN_L},
    {.pin = GPIO_BTN_PIN_D},
    {.pin = GPIO_BTN_PIN_R},
};

// btns actions
/**************************************************************************************************/

void btn_debounce(btn_t *const _btn, const uint32_t _period, const uint16_t _port_val) __attribute__((nonnull));

static __INLINE void _btn_event(btn_t *const _btn, const btn_event_t _event)
{
    if (_btn->event != NULL) _btn->event(_btn, _event);
}

void btn_debounce(btn_t *const _btn, const uint32_t _period, const uint16_t _port_val)
{
#if BTN_ACTIVE == 0
    const btn_event_t curr = (_port_val & _btn->pin) ? BTN_POP : BTN_PUSH;
#else
    const btn_event_t curr = (_port_val & _btn->pin) ? BTN_PUSH : BTN_POP;
#endif
    
    // Debounce
    if (_btn->prev_status != curr)
    {
        _btn->timer = (_btn->prev_status == BTN_NA) ? BTN_TOUT_DEB + 1 : 0;
        _btn->prev_status = curr;
    }
    else
    {
        _btn->timer += _period;
    }
    
    _btn->push_timer += _period;

    if (true
        && _btn->timer - _period < BTN_TOUT_DEB
        && _btn->timer >= BTN_TOUT_DEB
        )
    {
        _btn_event(_btn, curr); // BTN_POP, BTN_PUSH
        
        if (curr == BTN_PUSH)
        {
            _btn->dbl = (_btn->push_timer < BTN_DBL_TOUT) ? BTN_DBL : BTN_NA;
            _btn->push_timer = 0;
        }
    }
    
    if (true
        && _btn->push_timer - _period < BTN_DBL_TOUT
        && _btn->push_timer >= BTN_DBL_TOUT
        && _btn->timer < BTN_DBL_TOUT
        )
    {
       _btn_event(_btn, BTN_CLICK | _btn->dbl); // BTN_DBL_CLICK, BTN_CLICK
    }

    if (_btn->prev_status == BTN_PUSH)
    {
        if (true
            && _btn->timer - _period < BTN_LONG_TOUT
            && _btn->timer >= BTN_LONG_TOUT
            )
        {
            _btn_event(_btn, BTN_LONG_PRESS | _btn->dbl); // BTN_DBL_LONG_PRESS, BTN_LONG_PRESS
        }
        
        if (_btn->timer > BTN_REPEAT_TOUT)
        {
            _btn_event(_btn, BTN_REPEAT | _btn->dbl); // BTN_DBL_REPEAT, BTN_REPEAT
            _btn->timer -= BTN_REPEAT_INTERVAL;
        }
    }
}

/**************************************************************************************************/
// btns actions

// GPIO_LED blink
static void _blinker(const uint32_t _period)
{
    static uint32_t time = 0;
    time += _period;
    
    if (time >= 500)
    {
        uint16_t tmp = GPIO_ReadOutputData(GPIO_USR_LED_PORT);
        tmp ^= GPIO_USR_LED_PIN;
        GPIO_Write(GPIO_USR_LED_PORT, tmp);
        
        time -= 500;
    }
}


void TIM1_UP_IRQHandler(void)
{
    const uint32_t period  = 1000 / PWM_FREQ;
    const uint16_t btns_val = GPIO_ReadInputData(GPIO_BTN_PORT);

    if ((btns_val & BTN_PIN_MASK) == BTN_PIN_MASK)
    {
        _blinker(period);
    }
    
    for (size_t i = 0; i < (sizeof(btns) / sizeof(btns[0])); i++)
    {
        btn_debounce(&btns[i], period, btns_val);
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
