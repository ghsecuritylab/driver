#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <modbus.h>

#define RS485	GET_PIN(G, 8)
static rt_device_t dev;//串口设备
static struct MODBUS_timer RS485_timer;//定时器结构体
static rt_mutex_t rs485_lock = RT_NULL;//MODBUS 协议锁
static rs485_t message;//RS485 数据

/*	串口接收回调函数	*/
static rt_err_t modbus_data(rt_device_t dev,rt_size_t size)
{
	if(message.index>=1024)
	{
		message.report=1;
		return RT_FALSE; 
	}
	rt_device_read(dev,0,&message.buffer[message.index++],1);//接收一个字符
	rt_device_write(RS485_timer.dev_timer,0,&RS485_timer.timeout,sizeof(RS485_timer.timeout));//启动定时
	return RT_EOK;
}

/*	定时器超时回调函数	*/
static rt_err_t timer_out(rt_device_t dev,rt_size_t size)
{
	if(message.buffer[0] == SLAVE)//信息是发给该从机站的
	{
		rt_mutex_release(rs485_lock);
		rt_kprintf("从机接收\n");
	}
	else//信息不是发给该从机站的
	{
		rt_kprintf("从机拒绝\n");
		rt_memset(&message,0,sizeof(message));//清空结构体
	}
	rt_kprintf("定时器超时!\n");
	return RT_EOK;
}

/*	modbus 解析函数	*/
static void modbus_app(void *parameter)
{
	
	while(1)
	{
		rt_mutex_take(rs485_lock,RT_WAITING_FOREVER);
		if(message.index>0)
		{
			rs485_write(message.buffer,message.index);
		}
		rt_memset(&message,0,sizeof(message));//清空结构体
		
		rt_thread_delay(100);//线程延时
	}
}

int modbus_init()
{
	rt_pin_mode(RS485,PIN_MODE_OUTPUT);//485使能引脚
	rt_pin_write(RS485,PIN_LOW);
	
	rs485_lock = rt_mutex_create("RS485",RT_IPC_FLAG_FIFO);//初始化互斥量
	if(rs485_lock == RT_NULL)
	{
		rt_kprintf("modbus mutex create error!\n");
		return RT_ERROR;
	}
	
	/* 串口设备启用 */
	dev = rt_device_find("uart2");
	if(dev==RT_NULL)
	{
		rt_kprintf("RS485串口查找失败!\n");
		return RT_ERROR;
	}
	rt_device_open(dev,RT_DEVICE_FLAG_INT_RX);
	rt_device_set_rx_indicate(dev,modbus_data);//串口接收回调函数
	
	/* 定时器设备启用 */
	{
		RS485_timer.mode = HWTIMER_MODE_ONESHOT;
		RS485_timer.timeout.sec = 1;
		RS485_timer.timeout.usec = 0;
	}
	
	RS485_timer.dev_timer = rt_device_find("timer13");
	if(RS485_timer.dev_timer==RT_NULL)
	{
		rt_kprintf("timer13 find error!\n");
		return RT_ERROR;
	}
	
	if(rt_device_open(RS485_timer.dev_timer,RT_DEVICE_OFLAG_RDWR) != RT_EOK)
	{
		rt_kprintf("timer13 open error!\n");
		return RT_ERROR;
	}
	rt_device_set_rx_indicate(RS485_timer.dev_timer,timer_out);//定时器超时回调函数
	if(rt_device_control(RS485_timer.dev_timer,HWTIMER_CTRL_MODE_SET,&RS485_timer.mode))
	{
		rt_kprintf("timer13 mode set error!\n");
		return RT_ERROR;
	}
	
	{
		rt_thread_t tid = RT_NULL;
		tid = rt_thread_create("modbus",modbus_app,RT_NULL,1024,8,20);
		if(tid != RT_NULL)
			rt_thread_startup(tid);
		else
		{
			rt_kprintf("modbus thread create error!\n");
			return RT_ERROR;
		}
	}
	return RT_EOK;
}
INIT_PREV_EXPORT(modbus_init);



static rt_err_t rs485_write(rt_uint8_t *buffer,rt_uint16_t length)
{
	rt_pin_write(RS485,PIN_HIGH);
	rt_device_write(dev,0,buffer,length);
	rt_pin_write(RS485,PIN_LOW);
	return RT_EOK;
}
