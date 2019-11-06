#include <rtthread.h>
#include <rtdevice.h>
#include "AD7739.h"

#define LOG_TAG	"rocker"
#include <drv_log.h>

static AD7739_t rocker = RT_NULL;
static rt_sem_t rocker_lock = RT_NULL; 

rt_uint32_t rocker_buffer[8] = {0};


static void ready(void *args)
{
	rt_sem_release(rocker_lock);//释放信号量
}

static void rocker_entry(void *parameter)
{
	float temp = 0;
	rt_uint8_t data[24] = {0};
	rt_uint8_t flag = 0;
	while(1)
	{
		rt_sem_take(rocker_lock,RT_WAITING_FOREVER);//永久等待信号量
		rt_pin_irq_enable(rocker->ready_pin,PIN_IRQ_DISABLE);//停止AD7739中断，进行数据运算
		
		flag++;
		if((flag%10)==0)
		{
			ad7739_channel_read(rocker,data,3);//读所有通道数据
			for(int i=0;i<8;i++)//整合通道数据
			{
				int j=i*3;
				rocker_buffer[i] = (data[j]<<16) | (data[j+1]<<8) | (data[j+2]);
			}
			for(int i=0;i<8;i++)// 换算通道结果
			{
				temp = rocker_buffer[i] * 5.0 / (1<<24);
				rocker_buffer[i] = (rt_uint16_t)(temp * 100);
			}
		}
		
		
		
		rt_kprintf("%d %d %d %d %d %d %d %d\n",
				   rocker_buffer[0],rocker_buffer[1],rocker_buffer[2],rocker_buffer[3],
				   rocker_buffer[4],rocker_buffer[5],rocker_buffer[6],rocker_buffer[7]);
		
		rt_pin_irq_enable(rocker->ready_pin,PIN_IRQ_ENABLE);//计算结束，开启中断
	}
}

void rocker_init()
{
	rt_thread_t tid = RT_NULL;
	rocker = rt_calloc(1,sizeof(struct AD7739));
	if(rocker==RT_NULL)
	{
		LOG_E("rocker device memory apply failed!\n");
		ad7739_deinit(rocker);
		return;
	}
	
	rocker->adc_device_name = "rocker_device";
	rocker->channel_enable = 0xff;
	rocker->ready_pin = GET_PIN(B, 7);
	rocker->reset_pin = GET_PIN(B, 6);
	ad7739_init("spi1",rocker,GPIOB,GPIO_PIN_5);
	
	rocker_lock = rt_sem_create("rocker_lock",0,RT_IPC_FLAG_FIFO);
	if(rocker_lock == RT_NULL)
	{
		LOG_E("rocker_lock create falied!\n");
		rt_free(rocker_lock);
		ad7739_deinit(rocker);
		return;
	}	
	
	tid = rt_thread_create("rocker",rocker_entry,RT_NULL,1024,10,15);
	if(tid==RT_NULL)
	{
		LOG_E("rocker thread create failed!\n");
		rt_free(rocker_lock);
		ad7739_deinit(rocker);
		return;
	}
	
	rt_pin_attach_irq(rocker->ready_pin,PIN_IRQ_MODE_LOW_LEVEL,ready,RT_NULL);
	rt_pin_irq_enable(rocker->ready_pin,PIN_IRQ_ENABLE);
	
	rt_thread_startup(tid);
}
MSH_CMD_EXPORT(rocker_init,init rocker device);

