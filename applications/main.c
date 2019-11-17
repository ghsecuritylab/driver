/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-05     whj4674672   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
//#include "drv_lcd.h"
#include "c_power.h"
//#include "cllogo.h"

int main(void)
{
    int count = 1;
    rt_pin_mode(LED_RUN_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_ERR_PIN, PIN_MODE_OUTPUT);
    
    while (count++)
    {
        rt_thread_mdelay(200);
        
        rt_pin_write(LED_RUN_PIN, !rt_pin_read(LED_RUN_PIN));
        
        rt_pin_write(LED_ERR_PIN, !rt_pin_read(LED_ERR_PIN));
    }
    return RT_EOK;
}
