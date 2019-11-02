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
		rt_kprintf("test per 5s!\n");
		rt_thread_mdelay(3000);
    }
    return RT_EOK;
}

#include "spi_flash.h"
#include "spi_flash_sfud.h"
#include "drv_spi.h"

void flash_test()
{
	 __HAL_RCC_GPIOE_CLK_ENABLE();
    rt_hw_spi_device_attach("spi4", "spi40", GPIOE, GPIO_PIN_4);

    if (RT_NULL == rt_sfud_flash_probe("W25Q256", "spi40"))
    {
        rt_kprintf("sfud to W25Q256 failed!\n");
		return;
    }
	rt_kprintf("sufd to W25Q256 sucessful!\n");
    return ;
}
MSH_CMD_EXPORT(flash_test, W25Q256 device test);
