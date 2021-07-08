/**
 * @file appl_serial.h
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.07.04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

bool appl_serial_handle(char *const _str, const uint8_t _max_size);
