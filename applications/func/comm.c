#include <rtthread.h>
#include <rtdevice.h>
#include "comm.h"

rt_mailbox_t com = RT_NULL;
static msg *get;
static rt_device_t handle = RT_NULL;

static rt_uint32_t ai;
static rt_uint32_t di;
static rt_uint8_t a,b,c,d;
static rt_uint16_t e,f;

static void com_entry(void *parameter)
{
	while(1)
	{
		rt_mb_recv(com,(rt_ubase_t*)&get,RT_WAITING_FOREVER);// waiting message
		if(get->type==0){//rocker
			ai = get->buffer;
		}else if(get->type==1){	// switch
			di = get->buffer;
		}
		
		a=(rt_uint8_t)(ai);b=(rt_uint8_t)(ai>>8);c=(rt_uint8_t)(ai>>16);d=(rt_uint8_t)(ai>>24);
		e=(rt_uint16_t)(di);f=(rt_uint16_t)(di>>16);
		
		rt_kprintf("rocker:%d %d %d %d	switch:%04x %04x\n",
					a,b,c,d,e,f);
	}
}

int community()
{
	rt_thread_t tid = RT_NULL;
	com = rt_mb_create("community",1,RT_IPC_FLAG_FIFO);
	if(com==RT_NULL)
	{
		rt_kprintf("com mailbox create failed!\n");
		return RT_ERROR;
	}
	
	handle = rt_device_find("uart3");
	if(handle==RT_NULL)
	{
		rt_kprintf("usart3 device find failed!/n");
		return RT_ERROR;
	}
	rt_pin_mode(GET_PIN(D,10),PIN_MODE_OUTPUT);
	rt_pin_write(GET_PIN(D,10),PIN_HIGH);//Ê¹ÄÜ·¢ËÍ
	
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
INIT_APP_EXPORT(community/*,open community task*/);
