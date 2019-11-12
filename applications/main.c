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

int main(void)
{
    int count = 1;
	
    while (count++)
    {
//		rt_kprintf("test per 5s!\n");
		rt_thread_mdelay(3000);
    }
    return RT_EOK;
}
