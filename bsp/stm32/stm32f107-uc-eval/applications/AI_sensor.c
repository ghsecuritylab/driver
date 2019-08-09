#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <ad7739.h>
#include <center.h>

/*all channel data to store*/
ad_sensor sensor_center[12] = {0};


/*********self value*********/
static AD7739_t dev1;	//设备句柄
static AD7739_t dev2;	//两块 AD7739 芯片

static rt_uint8_t adc_channel_value[36] = {0};//

/*
* 读取一次AD7739,并转换数据
* 存入传感数组的 value 成员
*/
static rt_err_t sensor_update()
{
	rt_uint8_t ID = 0;
	rt_uint32_t temp = 0;
	ad7739_read(dev1,AD7739_READ|AD7739_REV,&ID,1);
	
	if((ID & 0x0f) == 0x09)
	{
		ad7739_channel_read(dev1,adc_channel_value,3);
		ad7739_channel_read(dev2,&adc_channel_value[24],3);
		
		for(int i=0;i<12;i++)
		{
			for(int j=(i*3);j<(i*3+3);j++)
			{
				temp = temp<<8|adc_channel_value[j]; 
			}
			
			sensor_center[i].sensor_value = (float)(temp*5)/0xffffff;
			temp = 0;
		}
		return RT_EOK;
	}
	else
	{
		return RT_ERROR;
	}
}

/*
* 传感器数据十次平均滤波
*/
static rt_err_t sensor_filt()
{
	float temp_buffer[12] = {0.0};
	
	/********filt by adding ten value********/
	for(int i=0;i<10;i++)
	{
		sensor_update();//Update sensor value
		
		for(int j=0;j<12;j++)
		{
			temp_buffer[j] += sensor_center[j].sensor_value;
		}
	}
	
	/**********get average value***********/
	for(int k=0;k<12;k++)
	{
		sensor_center[k].sensor_value = (temp_buffer[k]/10.0)
										* sensor_center[k].radio	// Multi radio value
										+ sensor_center[k].offset;	// Add offset value
		
		/*discard the extra low value*/
		if(sensor_center[k].sensor_value < 0.0001)
			sensor_center[k].sensor_value = 0.00;
		
		temp_buffer[k] = 0.0;
	}
	
	return RT_EOK;
}


static void ad7739_thread_entry(void *parameter)
{
	
	while(1)
	{
		sensor_filt();
		rt_thread_delay(500);
	}
}

int sensor_init()
{
	dev1 = rt_calloc(1,sizeof(AD7739_t));
	dev2 = rt_calloc(1,sizeof(AD7739_t));
	
	dev1->adc_device_name = "AD7739_1";
	dev1->channel_enable = 0xff;
	dev1->ready_pin = GET_PIN(A,1);
	dev1->reset_pin = GET_PIN(B,1);
	
	dev2->adc_device_name = "AD7739_2";
	dev2->channel_enable = 0x0f;
	dev2->ready_pin = GET_PIN(A,2);
	dev2->reset_pin = GET_PIN(A,4);
	
	ad7739_init("spi1",dev1,GPIOB,GPIO_PIN_0);
	ad7739_init("spi1",dev2,GPIOA,GPIO_PIN_3);
	
	{
		rt_thread_t tid;
		tid = rt_thread_create("ad7739_thread",ad7739_thread_entry,RT_NULL,1024,10,20);
		
		if(tid!=RT_NULL)
			rt_thread_startup(tid);
		else 
		{
			rt_kprintf("ad7739 thread create error!\n");
			rt_free(dev1);
			rt_free(dev2);
			return RT_ERROR;
		}
	}
	
	return RT_EOK;
}
INIT_APP_EXPORT(sensor_init);

