#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <ad7739.h>

static AD7739_t dev;

void sample_entry(void *parameter)
{
	rt_uint8_t cmd;
	rt_uint8_t buffer[24];
	
	while(1)
	{
		ad7739_read(dev,AD7739_REV,&cmd,1);
		ad7739_channel_read(dev,buffer,3);
	
		rt_thread_delay(1000);
	}
}

int adc_sample(void)
{
	dev = rt_calloc(1,sizeof(struct AD7739));
	
	dev->adc_device_name = "AD7739";
	dev->channel_enable	 = 0x03;
	dev->ready_pin		 = GET_PIN(B,7);
	dev->reset_pin		 = GET_PIN(B,6);
	
	ad7739_init("spi1",dev,GPIOB,GPIO_PIN_5);
	
	{
		rt_thread_t tid;
		
		tid = rt_thread_create("adc_task",sample_entry,RT_NULL,1024,8,20);
		
		if(tid != RT_NULL)
			rt_thread_startup(tid);
	}
	return RT_EOK;
}
MSH_CMD_EXPORT(adc_sample, ad7739 device init);

