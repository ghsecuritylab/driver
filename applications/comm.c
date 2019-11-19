#include "c_power.h"

static rt_device_t handle = RT_NULL;

static rt_uint32_t ai;
static rt_uint32_t di;
static rt_uint8_t a,b,c,d;
static rt_uint16_t e,f;

static void value_deal(void);

static void com_entry(void *parameter)
{
	lcd_show_string(0,80,24,"%s"," B A O H L T F P");
	lcd_show_string(0,140,24,"%s"," S  O  T  C  P  R");
    while(1)
    {
        rt_thread_mdelay(10);
		ai = rocker_get();
		di = button_get();
		
        a=(rt_uint8_t)(ai);
        b=(rt_uint8_t)(ai>>8);
        c=(rt_uint8_t)(ai>>16);
        d=(rt_uint8_t)(ai>>24);
        e=(rt_uint16_t)(di);
        f=(rt_uint16_t)(di>>16);
		lcd_show_string(0,55,24," %03d %03d %03d %03d",c,d,a,b);//左履带 右履带 截割臂升降 截割臂转动
//        rt_kprintf("rocker:%d %d %d %d	switch:%04x %04x\n",
//                   a,b,c,d,e,f);
//		rt_kprintf("rocker:%d %d %d %d\n",a,b,c,d);
		
		value_deal();
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

static void value_deal(void)
{
	if(e&(1<<0))
			lcd_show_string(12,105,24,"%s","1");
		else 
			lcd_show_string(12,105,24,"%s","0");
		
	if(e&(1<<1))
		lcd_show_string(36,105,24,"%s","1");
	else 
		lcd_show_string(36,105,24,"%s","0");
	
	if(e&(1<<2))
		lcd_show_string(60,105,24,"%s","1");
	else if(e&(1<<3))
		lcd_show_string(60,105,24,"%s","0");
	
	if(e&(1<<4))
		lcd_show_string(84,105,24,"%s","1");
	else if(e&(1<<5))
		lcd_show_string(84,105,24,"%s","0");
	
	if(e&(1<<6))
		lcd_show_string(108,105,24,"%s","1");
	else if(e&(1<<7))
		lcd_show_string(108,105,24,"%s","0");
	
	if(e&(1<<8))
		lcd_show_string(132,105,24,"%s","1");
	else if(e&(1<<9))
		lcd_show_string(132,105,24,"%s","0");
	
	if(e&(1<<10))
		lcd_show_string(156,105,24,"%s","1");
	else if(e&(1<<11))
		lcd_show_string(156,105,24,"%s","0");
	
	if(e&(1<<12))
		lcd_show_string(180,105,24,"%s","1");
	else if(e&(1<<13))
		lcd_show_string(180,105,24,"%s","0");
	
	if(f==0){
		lcd_show_string(0,164,24,"%s"," 0  0  0  0  0  0 ");
		return;
	}
	
	if(f&(1<<0))
		lcd_show_string(120,164,24,"%s","1");
	else if(f&(1<<1))
		lcd_show_string(120,164,24,"%s","-1");
	
	if(f&(1<<2))
		lcd_show_string(12,164,24,"%s","1");
	else if(f&(1<<3))
		lcd_show_string(12,164,24,"%s","-1");	
	
	if(f&(1<<4))
		lcd_show_string(48,164,24,"%s","1");
	else if(f&(1<<5))
		lcd_show_string(48,164,24,"%s","-1");
	
	if(f&(1<<6))
		lcd_show_string(84,164,24,"%s","1");
	else if(f&(1<<7))
		lcd_show_string(84,164,24,"%s","-1");
	
	if(f&(1<<8))
		lcd_show_string(156,164,24,"%s","1");
	else if(f&(1<<9))
		lcd_show_string(156,164,24,"%s","-1");
	
	if(f&(1<<10))
		lcd_show_string(192,164,24,"%s","1");
	else if(f&(1<<11))
		lcd_show_string(192,164,24,"%s","-1");
}
