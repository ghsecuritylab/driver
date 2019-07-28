/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   change to new framework
 * 2019-01-12     whj          add stm32f107-uc-Eval bsp
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


/* defined the LED1 pin: PD13 */
#define LED1_PIN    GET_PIN(E, 0)
#define LED0_PIN	GET_PIN(E, 1)

int main(void)
{
    int count = 1;
    /* set LED1 pin mode to output */
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
	
    while (count++)
    {	
        rt_pin_write(LED1_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED1_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }

    return RT_EOK;
}

int rs485_send_enable()
{
	rt_pin_mode(GET_PIN(D,7),PIN_MODE_OUTPUT);
	rt_pin_write(GET_PIN(D,7),PIN_HIGH);
	return 0;
}
INIT_BOARD_EXPORT(rs485_send_enable);
