/**
 * @file appl_btn.h
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.06.30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <stdint.h>


void appl_btn_init(const uint16_t _grade);
void appl_btn_handle(const uint32_t _period, const uint16_t _port_val);
