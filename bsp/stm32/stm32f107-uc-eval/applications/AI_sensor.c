#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <ad7739.h>

typedef struct ad_sensor
{
	float sensor_value;
	const float radio;
	float offset;
	const float full;
	const float zero;
}ad_sensor;

ad_sensor sensor_center[12] = 
{
	{0.0,1.0,0.0,20.0,0.0},
	{0.0,1.0,0.0,20.0,0.0},
	{0.0,1.0,0.0,20.0,0.0},
	{0.0,1.0,0.0,20.0,0.0},
	{0.0,1.0,0.0,20.0,0.0},
	{0.0,1.0,0.0,20.0,0.0},
	{0.0,1.0,0.0,20.0,0.0},
	{0.0,1.0,0.0,20.0,0.0},
	{0.0,1.0,0.0,20.0,0.0},
	{0.0,1.0,0.0,20.0,0.0},
	{0.0,1.0,0.0,20.0,0.0},
	{0.0,1.0,0.0,20.0,0.0}
};

/*****allow extern change*****/
float sensor_radio[12] = {0.0}; 
float sensor_offset[12] = {0.0}; 


/*********self value*********/
static AD7739_t dev;//Éè±¸¾ä±ú

static float sensor_value[12] = {0.0};
static rt_uint8_t adc_channel_value[24] = {0};

static rt_err_t sensor_update()
{
	rt_uint8_t ID = 0;
	rt_uint32_t temp = 0;
	ad7739_read(dev,AD7739_READ|AD7739_REV,&ID,1);
	if((ID & 0x0f) == 0x09)
	{
		ad7739_channel_read(dev,adc_channel_value,3);
		
		for(int i=0;i<8;i++)
		{
			for(int j=(i*3);j<(i*3+3);j++)
			{
				temp = temp<<8|adc_channel_value[j]; 
			}
//			sensor_value[i] = (float)(temp*5)/0xffffff;
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

static rt_err_t sensor_filt()
{
	float temp_buffer[12] = {0.0};
	
	/********filt by adding ten value********/
	for(int i=0;i<10;i++)
	{
		sensor_update();//Update sensor value
		
		for(int j=0;j<12;j++)
		{
//			temp_buffer[j] += sensor_value[j];
			temp_buffer[j] += sensor_center[j].sensor_value;
		}
	}
	
	/**********get average value***********/
	for(int k=0;k<12;k++)
	{
//		sensor_value[k] = temp_buffer[k]/10.0;
		sensor_center[k].sensor_value = temp_buffer[k]/10.0;
		sensor_center[k].sensor_value = sensor_center[k].sensor_value
										* sensor_center[k].radio	// Multi radio value
										+ sensor_center[k].offset;	// Add offset value
		
		/*discard the extra low value*/
//		if(sensor_value[k]<0.0001)
//			sensor_value[k]=0.00;
//		else 
//			sensor_value[k] += 0.05;
		if(sensor_center[k].sensor_value < 0.0001)
			sensor_center[k].sensor_value = 0.00;
		else
			sensor_center[k].sensor_value += 0.05;
		
		temp_buffer[k] = 0.0;
	}
	
	return RT_EOK;
}

static rt_err_t sensor_deal()
{
	
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
	dev = rt_calloc(1,sizeof(AD7739_t));
	
	dev->adc_device_name = "AD7739";
	dev->channel_enable = 0xff;
	dev->ready_pin = GET_PIN(A,1);
	dev->reset_pin = GET_PIN(B,1);
	
	ad7739_init("spi1",dev,GPIOB,GPIO_PIN_0);
	
	{
		rt_thread_t tid;
		tid = rt_thread_create("ad7739_thread",ad7739_thread_entry,RT_NULL,1024,10,20);
		
		if(tid!=RT_NULL)
			rt_thread_startup(tid);
		else 
		{
			rt_kprintf("ad7739 thread create error!\n");
			rt_free(dev);
			return RT_ERROR;
		}
	}
	
	return RT_EOK;
}
INIT_APP_EXPORT(sensor_init);

