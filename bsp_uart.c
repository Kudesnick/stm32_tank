/**
 * @file bsp_uart.c
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.07.04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "RTE_Components.h"
#include CMSIS_device_header

#include "bsp_uart.h"
#include "bsp_gpio.h"


#define UART            USART2
#define UART_RCC        RCC_APB1Periph_USART2
#define UART_IRQ        USART2_IRQn

#define UART_PORT_RCC   RCC_APB2Periph_GPIOA
#define UART_PORT       GPIOA
#define UART_RX         GPIO_Pin_3
#define UART_TX         GPIO_Pin_2


#define UART_DMA_TX
#define UART_DMA_TX_RCC
#define UART_DMA_TX_CH


void USART2_IRQHandler(void)
{
    volatile static uint8_t i = 0;

    if (USART_GetFlagStatus(UART, USART_FLAG_RXNE) == SET)
    {
        i = UART->DR;
    }
    if (USART_GetFlagStatus(UART, USART_FLAG_IDLE) == SET)
    {
        i = UART->DR;
    }
    
    USART_ClearFlag(UART, USART_FLAG_RXNE | USART_FLAG_IDLE);
}


void bsp_uart_int(const uint32_t _baud)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | UART_PORT_RCC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure =
    {
        .GPIO_Speed = GPIO_Speed_2MHz,
        .GPIO_Mode  = GPIO_Mode_AF_PP,
        .GPIO_Pin   = UART_RX | UART_TX,
    };
    GPIO_Init(UART_PORT, &GPIO_InitStructure);

    RCC_APB1PeriphClockCmd(UART_RCC, ENABLE);
    USART_InitTypeDef USART_InitStructure =
    {
        .USART_BaudRate = _baud,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
        .USART_WordLength = USART_WordLength_8b,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity_No,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
    };
    USART_Init(UART, &USART_InitStructure);
    USART_ClockInitTypeDef USART_ClockInitStructure;
    USART_ClockStructInit(&USART_ClockInitStructure);
    USART_ClockInit(UART, &USART_ClockInitStructure);
    NVIC_EnableIRQ(UART_IRQ);
    USART_ITConfig(UART, USART_IT_RXNE | USART_IT_IDLE, ENABLE);

//    USART_DMACmd(UART, USART_DMAReq_Rx, ENABLE);
    USART_Cmd(UART, ENABLE);
}
