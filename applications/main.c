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

/* defined the LCD_BLK pin: PD13 */
#define LCD_BLK    GET_PIN(D, 13)

int main(void)
{
    int count = 1;
    /* set LCD_BLK pin mode to output */	
    rt_pin_mode(LCD_BLK, PIN_MODE_OUTPUT);	
	
    while (count++)
    {
        rt_pin_write(LCD_BLK, PIN_HIGH);
        rt_thread_mdelay(1000);
        rt_pin_write(LCD_BLK, PIN_LOW);
        rt_thread_mdelay(1000);
    }
    return RT_EOK;
}
