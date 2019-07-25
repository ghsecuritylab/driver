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

#include <led.h>

/* defined the LED1 pin: PD13 */
#define LED1_PIN    GET_PIN(E, 0)
#define LED0_PIN	GET_PIN(E, 1)

int main(void)
{
    int count = 1;
    /* set LED1 pin mode to output */
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);

//	led_init();
//	led_start(LED0_PIN,LED_FAST);
	
    while (count++)
    {
        rt_pin_write(LED1_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED1_PIN, PIN_LOW);
        rt_thread_mdelay(500);
//		if(count==10)
//			led_stop(LED0_PIN);
//		if(count==20)
//			led_start(LED0_PIN,LED_SLOW);
//		if(count==30)
//			led_stop(LED0_PIN);
//		if(count==40)
//			led_start(LED0_PIN,LED_ONE);
//		if(count==50)
//			led_stop(LED0_PIN);
//		if(count==60)
//			led_start(LED0_PIN,LED_DOUBLE);
//		if(count==70)
//			led_stop(LED0_PIN);
//		if(count==80)
//			led_start(LED0_PIN,LED_THREE);
//		if(count==90)
//			led_stop(LED0_PIN);
//		if(count==100)
//			led_start(LED0_PIN,LED_VERY_SLOW);
//		if(count==110)
//			led_stop(LED0_PIN);
//		if(count==120)
//			led_start(LED0_PIN,LED_ON);
//		if(count==130)
//			led_stop(LED0_PIN);
//		if(count==140)
//			led_start(LED0_PIN,LED_OFF);
//		if(count==150)
//			led_stop(LED0_PIN);
    }

    return RT_EOK;
}
