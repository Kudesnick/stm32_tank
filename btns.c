/**
 * @file btns.c
 * @author Stulov Tikhon (kudesnick@inbox.ru)
 * @brief 
 * @version 0.1
 * @date 21.06.29
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#include "RTE_Components.h"
#include CMSIS_device_header


#include <stdlib.h>
#include "btns.h"


#define BTN_ACTIVE          (1)
#define BTN_TOUT_DEB        (40)
#define BTN_DBL_TOUT        (400)
#define BTN_LONG_TOUT       (500)
#define BTN_REPEAT_TOUT     (1500)
#define BTN_REPEAT_INTERVAL (500)


static __INLINE void _btn_event(btn_t *const _btn, const btn_event_t _event)
{
    if (_btn->event != NULL) _btn->event(_btn, _event);
}


void btn_handle(btn_t *const _btn, const uint32_t _period, const uint16_t _port_val)
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
