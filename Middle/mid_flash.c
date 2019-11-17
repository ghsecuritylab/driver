/*
******************************************************************************************************
*
*  Copyright (C),   2004-2019, Shanghai Chuangli Group Co.Ltd.
*  FileName:        mid_flash.c
*  Version:         ver1.0
*  Author:          王海靖
*  Date:            2019-11-17
*  Description:     按键控制模块
*  Maintainers:     none
*                            
******************************************************************************************************
*/

#include "c_power.h"

#define DBG_ENABLE 
#define DBG_SECTION_NAME "mid.flash"
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR
#include <rtdbg.h> 

int hw_spi_flash_init(void)
{
    rt_device_t device;
    
    rt_hw_spi_device_attach("spi4", "spi40",GPIOE, GPIO_PIN_4);

    if (RT_NULL == rt_sfud_flash_probe("W25Q256", "spi40"))
    {
        LOG_E("not found flash");
        return -RT_ERROR;
    };

    device = rt_device_find("W25Q256");
    while (device != RT_NULL)
    {
        if (dfs_mount("W25Q256", "/", "elm", 0, 0) == RT_EOK)
        {
            LOG_I("W25Q256 mount to '/' succes\n");
            
            mkdir("/sd", 0x777);

            return RT_EOK;
        }
        else
        {
            dfs_mkfs("elm","W25Q256");
            LOG_E("W25Q256 mount to '/' failed!\n");
        }
    }
    return RT_EOK;
}
