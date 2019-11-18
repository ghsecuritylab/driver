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
#include "c_power.h"
#include "drv_lcd.h"

static void time_show(void)
{
    time_t t;
    struct tm *lt;
    char* times;
    char time1[25]={0};
    t = time(NULL);  //获取当前时间，单位是秒
    lt = localtime(&t);//转换成tm类型的结构体;
    times = asctime(lt);
    strncpy(time1,times,24);
    lcd_show_string(0, 224, 16, time1);
}

int main(void)
{
    int count = 1;
    rt_pin_mode(LED_RUN_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_ERR_PIN, PIN_MODE_OUTPUT);

    while (count++)
    {
        time_show();
        
        rt_thread_mdelay(500);
        
        rt_pin_write(LED_RUN_PIN, !rt_pin_read(LED_RUN_PIN));
        
        rt_pin_write(LED_ERR_PIN, !rt_pin_read(LED_ERR_PIN));
    }
    return RT_EOK;
}
