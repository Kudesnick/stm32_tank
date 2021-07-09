/**
 * @file main.h
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.06.30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#define BLINK_INTERVAL  (1000) // (ms)
#define PWM_GRADE       (256)
#define PWM_FREQ        (200)   // (Hz)
#define PWM_MIN_DUTY    (64)
#define BUF_SIZE        (32)
// #define UART_BAUD       (57600) // Debug only, ST-Link VCP doesn't work at a higher baudrate
#define UART_BAUD       (115200)
