#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <led.h>

const static rt_uint16_t led_stander_value[8] = 			//控制闪烁时间
					{0,0,0x2121,0x2424,0x1F23,0x1F42,0x1F62,0x2D2D};
					
#ifdef PIN_HIGH_ENABLE
/*
const static rt_uint16_t led_stander_value[8] = 			//控制闪烁时间
						{0,0,0x2121,0x2424,0x1A22,0x1942,0x1562,0x2A2A};
*/
#define LED_LIGHT	PIN_HIGH
#define LED_DARK	PIN_LOW
#else

#define LED_LIGHT	PIN_LOW
#define LED_DARK	PIN_HIGH

#endif
					
static rt_thread_t tid;//LED task thread 句柄

/*					
static rt_timer_t  tim;//LED task timer	 句柄
static rt_sem_t	   sem;//LED task sem	 句柄
*/
					
static struct led led_list[led_amount] = {0};


rt_err_t led_start(rt_base_t pin_num,LED_TYPE led_flash)
{
//	rt_pin_mode(pin_num,PIN_MODE_OUTPUT);
	
	if(led_flash==LED_OFF)//常灭
	{
		rt_pin_write(pin_num,LED_DARK);
	}
	else if(led_flash==LED_ON)//常亮
	{
		rt_pin_write(pin_num,LED_LIGHT);
	}
	else
	{
		for(int i=0;i<led_amount;i++)//寻找一个空的结构体使用
		{
			if(led_list[i].pin_number==0)//赋值
			{
				led_list[i].pin_number = pin_num;
				led_list[i].pin_stander = led_stander_value[led_flash];
				led_list[i].pin_cont = 0;
				break;
			}
		}
	}
	return RT_EOK;
}


void led_stop(rt_base_t pin_num)
{
	
	for(int i=0;i<led_amount;i++)
	{
		if(pin_num==led_list[i].pin_number)
		{
			rt_memset(&led_list[i],0,sizeof(led_list[i]));
		}
	}
	
	rt_pin_write(pin_num,LED_DARK);
}

static void led_task(void *parameter)
{	
	while(1)
	{
//		rt_sem_take(sem,RT_WAITING_FOREVER);
		
		for(int i=0;i<led_amount;i++)
		{
			if(led_list[i].pin_number!=0)
			{
				led_list[i].pin_cont++;
				if((led_list[i].pin_cont & 0x0f) == (led_list[i].pin_stander & 0x0f))
				{
					led_list[i].pin_cont = led_list[i].pin_cont & 0xf0;
					led_list[i].pin_cont = led_list[i].pin_cont + (1<<4);
					
#ifdef PIN_HIGH_ENABLE
						rt_pin_write(led_list[i].pin_number,(led_list[i].pin_cont>>4)&0x01);
#else
						rt_pin_write(led_list[i].pin_number,((led_list[i].pin_cont>>4)+1)&0x01);					
#endif	
					
					if((led_list[i].pin_cont & 0xf0) == (led_list[i].pin_stander & 0xf0))
					{
						led_list[i].pin_stander = led_list[i].pin_stander>>8 | led_list[i].pin_stander<<8;
						led_list[i].pin_cont = 0;
						rt_pin_write(led_list[i].pin_number,LED_DARK);
					}
				}
			}
		}	
		rt_thread_delay(100);
	}
}

/*
static void led_timerout_entry(void *parameter)
{
	rt_sem_release(sem);
}
*/

int led_init(void)
{	
/*	
	sem = rt_sem_create("led_sem",0,RT_IPC_FLAG_FIFO);//先进先出
	
	tim = rt_timer_create("led_timer",led_timerout_entry,RT_NULL,100,
							RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
*/
	
	{
		rt_pin_mode(89,PIN_MODE_OUTPUT);
		rt_pin_write(89,LED_DARK);
		rt_pin_mode(90,PIN_MODE_OUTPUT);
		rt_pin_write(90,LED_DARK);
	}
	
	tid = rt_thread_create("led_task",led_task,RT_NULL,1024,10,20);
	
	rt_thread_startup(tid);
//	rt_timer_start(tim);
	
	if(tid == RT_NULL)
	{
		rt_kprintf("led task create error!\n");
		return RT_ERROR;
	}
	else
	{
		return RT_EOK;
	}
}


void led_deinit(void)
{
	RT_ASSERT(tid!=RT_NULL);
//	RT_ASSERT(tim!=RT_NULL);
//	RT_ASSERT(sem!=RT_NULL);
	
	rt_thread_delete(tid);
//	rt_timer_delete(tim);
//	rt_sem_delete(sem);
}



