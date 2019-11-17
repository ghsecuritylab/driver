/*
******************************************************************************************************
*
*  Copyright (C),   2004-2019, Shanghai Chuangli Group Co.Ltd.
*  FileName:        mid_fram.c
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
#define DBG_SECTION_NAME "mid.fram"
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR
#include <rtdbg.h> 

static fm25cl64_t dev;
static rt_uint8_t fm_protect = 0x08;   // protect the upper 1/4 memory area ,from 0x1800 to 0x1fff
static rt_uint8_t fm_unprotect = 0x00;

#define FM_addr_start 0x1800
static uint8_t fm_buff_write[8] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
static uint8_t fm_buff_read[8] = {0,0,0,0,0,0,0,0};
    
static void fm_entry(void *parameter)
{
    fm25cl64_WRSR(dev,&fm_unprotect);//取消写保护
    
    fm25cl64_MemoryWrite(dev,FM_addr_start,fm_buff_write,8);
    fm25cl64_WRSR(dev,&fm_protect);//开启写保护    
    fm25cl64_MemoryRead(dev,FM_addr_start,fm_buff_read,8);
    
    rt_kprintf("fm25cl64 read data %x %x %x %x %x %x %x %x\n",
                fm_buff_read[0],fm_buff_read[1],fm_buff_read[2],fm_buff_read[3],
                fm_buff_read[4],fm_buff_read[5],fm_buff_read[6],fm_buff_read[7]);
    
    fm25cl64_RDSR(dev,fm_buff_read);
}

int fram_sample(int argc,char** argv)
{
    rt_thread_t tid;
    
    tid = rt_thread_create("fm25cl64",fm_entry,RT_NULL,1024,10,20);
    if(tid!=RT_NULL)
       rt_thread_startup(tid);

    return RT_EOK;
}
MSH_CMD_EXPORT(fram_sample,fram sample);

int hw_fram_init(void)
{
    rt_err_t resault  = RT_ERROR;

    dev = rt_calloc(1,sizeof(fm25cl64_t));

    dev->bus_name = "spi4";
    dev->device_name = "fm25cl64";

    resault = fm25cl64_init(dev,GPIOE,GPIO_PIN_3);
    if(resault==RT_ERROR)
    {
        LOG_E("fm25cl64 device init failed!\n");
        rt_free(dev);
    }
    else
    {
        LOG_I("fm25cl64 device init success!");
    }
    
    return resault;
}
