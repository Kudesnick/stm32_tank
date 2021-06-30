/**
 * @file btns.h
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.06.29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>



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


void btn_handle(btn_t *const _btn, const uint32_t _period, const uint16_t _port_val) __attribute__((nonnull));
