/**
 * @file bsp_uart.h
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.07.04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef bool(*uart_callback_t)(char *const _str, const uint8_t _max_size);

void bsp_uart_int(const uint32_t _baud, uint8_t *const _buf_ptr, const uint8_t _size);
void bsp_uart_register_callback(const uart_callback_t _call);
