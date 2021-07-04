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


#define UART_DMA_RX     DMA1_Channel6
#define UART_DMA_RX_RCC RCC_AHBPeriph_DMA1

uint8_t buf_rx[32];

void USART2_IRQHandler(void)
{
    if (USART_GetFlagStatus(UART, USART_FLAG_IDLE) == SET)
    {
        uint8_t i = sizeof(buf_rx) - DMA_GetCurrDataCounter(UART_DMA_RX);
        if (i > 0 && buf_rx[i-1] == '\r')
        {
            DMA_Cmd(UART_DMA_RX, DISABLE);
            DMA_SetCurrDataCounter(UART_DMA_RX, sizeof(buf_rx));
            DMA_Cmd(UART_DMA_RX, ENABLE);
            buf_rx[i-1] = '\0';
        }
        USART_ReceiveData(UART);
    }
}

void bsp_uart_int(const uint32_t _baud)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | UART_PORT_RCC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure =
    {
        .GPIO_Speed = GPIO_Speed_2MHz,
        .GPIO_Mode  = GPIO_Mode_AF_OD,
        .GPIO_Pin   = UART_RX | UART_TX,
    };
    GPIO_Init(UART_PORT, &GPIO_InitStructure);

    RCC_AHBPeriphClockCmd(UART_DMA_RX_RCC, ENABLE);
    DMA_InitTypeDef DMA_InitStructure =
    {
        .DMA_PeripheralBaseAddr = (uint32_t)(&UART->DR),
        .DMA_MemoryBaseAddr = (uint32_t)buf_rx,
        .DMA_DIR = DMA_DIR_PeripheralSRC,
        .DMA_BufferSize = sizeof(buf_rx),
        .DMA_PeripheralInc = DMA_PeripheralInc_Disable,
        .DMA_MemoryInc = DMA_MemoryInc_Enable,
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
        .DMA_MemoryDataSize = DMA_MemoryDataSize_Byte,
        .DMA_Mode = DMA_Mode_Normal,
        .DMA_Priority = DMA_Priority_Low,
        .DMA_M2M = DMA_M2M_Disable,
    };
    DMA_Init(UART_DMA_RX, &DMA_InitStructure);
    DMA_Cmd(UART_DMA_RX, ENABLE);

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

    NVIC_EnableIRQ(UART_IRQ);
    USART_ITConfig(UART, USART_IT_IDLE, ENABLE);

    USART_DMACmd(UART, USART_DMAReq_Rx, ENABLE);
    USART_Cmd(UART, ENABLE);
}
