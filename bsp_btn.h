/**
 * @file bsp_btn.h
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.06.29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

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

void bsp_btn_init(void);
uint16_t bsp_btn_get_port_val(void);
