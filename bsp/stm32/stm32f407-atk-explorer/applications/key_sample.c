#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <key_scan.h>

rt_uint8_t value[key_amount] = {0};

void sample_entry(void *parameter)
{
	rt_device_t dev;
	
	dev = rt_device_find(key_device_name);
	
	if(dev==RT_NULL)
		rt_kprintf("key device find error!\n");
	
	rt_device_init(dev);
	rt_device_open(dev,1);
	
	while(1)
	{
		rt_device_read(dev,0,value,key_amount);
		
		rt_kprintf("%d %d %d",value[0],value[1],value[2]);
		
		rt_thread_delay(2000);
	}
}

int sample(void)
{
	
	rt_hw_key_init();
	
	{
		rt_thread_t tid;
		
		tid = rt_thread_create("key_task",sample_entry,RT_NULL,1024,8,20);
		
		if(tid != RT_NULL)
			rt_thread_startup(tid);
		else rt_kprintf("led task create error!\n");
	}
	
	return RT_EOK;
}
INIT_APP_EXPORT(sample);
