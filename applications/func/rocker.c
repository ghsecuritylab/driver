#include <rtthread.h>
#include <rtdevice.h>
#include "AD7739.h"

/**** Useing C Library ****/
#include <math.h>

#define LOG_TAG	"rocker"
#include <drv_log.h>

static AD7739_t rocker = RT_NULL;
static rt_sem_t rocker_lock = RT_NULL; 

static void direction(rt_uint16_t x,rt_uint16_t y);

rt_uint16_t resault[8] = {0};

static void ready(void *args)
{
	rt_sem_release(rocker_lock);//释放信号量
}

static void rocker_entry(void *parameter)
{
	float temp = 0;
	rt_uint8_t data[24] = {0};
	rt_uint32_t rocker_buffer[8] = {0};
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
				resault[i] += (rt_uint16_t)(temp * 100);
				resault[i] = resault[i] / 2;// 两次相加取均值
			}
			
			if(flag == 50)
			{
				flag = 0;//复位计数
				direction(resault[0],resault[1]);
/*				rt_kprintf("%d %d %d %d %d %d %d %d\n",// 输出结果
				   resault[0],resault[1],resault[2],resault[3],
				   resault[4],resault[5],resault[6],resault[7]);
*/			}
		}
		rt_thread_mdelay(10);// 让出CPU 10ms，防止中断长时间占用CPU
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
	
	rt_pin_attach_irq(rocker->ready_pin,PIN_IRQ_MODE_FALLING,ready,RT_NULL);//bind ready pin irq
	rt_pin_irq_enable(rocker->ready_pin,PIN_IRQ_ENABLE);// enable irq
	
	rt_thread_startup(tid);
}
MSH_CMD_EXPORT(rocker_init,init rocker device);


static void direction(rt_uint16_t x,rt_uint16_t y)
{
	float Xt,Yt = 0.0;
	rt_uint16_t r = 0;
	float arg = 0.0;
	rt_uint16_t deg = 0;
	
	/*	误差范围内认为原点	*/
	if((x>240)&&(x<260))
		x = 250;
	if((y>240)&&(y<260))
		y = 250;
	
	/*	Count Distance From (0,0)	*/
	Xt = x - 250.0;
	Yt = y - 250.0;
	r = (rt_uint16_t)sqrt(Xt*Xt + Yt*Yt);
	r = r*100/200;// r%
	if(r>100)
		r=100;// 限制最大值
	
	/*	Angle Translation */
	arg = atanf(Yt/Xt);
	arg = fabsf(arg);	// Absolutely Radian Value
	deg = (rt_uint16_t)(arg*180/3.14); // Angle Value
	
	/*		attach position		*/
	if (x==250)// y轴上
	{
		if(y==250)
		{
			rt_kprintf("arg:%d ,strangth:%d%\n",0,0);
		}
		else if(y>250)
		{
			rt_kprintf("arg:%d ,strangth:%d%\n",90,r);
		}
		else if(y<250)
		{
			rt_kprintf("arg:%d ,strangth:%d%\n",270,r);
		}
	}
	else if(x>250)// x轴正向侧
	{
		if(y>=250)//第1象限
		{
			rt_kprintf("arg:%d ,strangth:%d%\n",deg,r);
		}
		else if(y<250)//第4象限
		{
			rt_kprintf("arg:%d ,strangth:%d%\n",360-deg,r);
		}
	}
	else if(x<250)// x轴负向侧
	{
		if(y>=250)//第2象限
		{
			rt_kprintf("arg:%d ,strangth:%d%\n",180-deg,r);
		}
		else if(y<250)//第3象限
		{
			rt_kprintf("arg:%d ,strangth:%d%\n",180+deg,r);
		}
	}
}

