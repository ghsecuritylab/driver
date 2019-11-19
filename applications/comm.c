#include "c_power.h"

static rt_device_t handle = RT_NULL;

static rt_uint32_t ai;
static rt_uint32_t di;
static rt_uint8_t a,b,c,d;
static rt_uint16_t e,f;

static void com_entry(void *parameter)
{
    while(1)
    {
        rt_thread_mdelay(100);
		ai = rocker_get();
		di = button_get();
		
        a=(rt_uint8_t)(ai);
        b=(rt_uint8_t)(ai>>8);
        c=(rt_uint8_t)(ai>>16);
        d=(rt_uint8_t)(ai>>24);
        e=(rt_uint16_t)(di);
        f=(rt_uint16_t)(di>>16);
		lcd_show_string(0,60,24,"%03d %03d %03d %03d",c,d,a,b);//左履带 右履带 截割臂升降 截割臂转动
//        rt_kprintf("rocker:%d %d %d %d	switch:%04x %04x\n",
//                   a,b,c,d,e,f);
//		rt_kprintf("rocker:%d %d %d %d\n",a,b,c,d);
		lcd_show_string(0,100,24,"%s","sknkb");
    }
}

int community()
{
    rt_thread_t tid = RT_NULL;
	
    handle = rt_device_find("uart3");
    if(handle==RT_NULL)
    {
        rt_kprintf("usart3 device find failed!/n");
        return RT_ERROR;
    }
    rt_pin_mode(GET_PIN(D,10),PIN_MODE_OUTPUT);
    rt_pin_write(GET_PIN(D,10),PIN_HIGH);//使能发送

    tid = rt_thread_create("com",com_entry,RT_NULL,1024,7,10);
    if(tid==RT_NULL)
    {
        rt_kprintf("com thread create failed!\n");
        return RT_ERROR;
    }
    rt_thread_startup(tid);
    rt_kprintf("com thread create success!\n");
    return RT_EOK;
}

