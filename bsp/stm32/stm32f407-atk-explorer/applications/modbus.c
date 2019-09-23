#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <modbus.h>

#define RS485	GET_PIN(G, 8)
static rt_device_t dev;//串口设备
static struct MODBUS_timer RS485_timer;//定时器结构体
static rt_sem_t rs485_lock = RT_NULL;//MODBUS 协议锁
static rs485_t message;//RS485 数据

/*	串口接收回调函数	*/
static rt_err_t modbus_data(rt_device_t dev,rt_size_t size)
{
	if(message.RxIndex>=Message_MAX)
	{
		message.Rsp = Error_Value;
		message.RxIndex = Message_MAX-1;
	}
	rt_device_read(dev,0,&message.RxBuffer[message.RxIndex++],1);//接收一个字符
	rt_device_write(RS485_timer.dev_timer,0,&RS485_timer.timeout,sizeof(RS485_timer.timeout));//启动定时
	return RT_EOK;
}

/*	定时器超时回调函数	*/
static rt_err_t timer_out(rt_device_t dev,rt_size_t size)
{
	if((message.RxBuffer[0] == SLAVE) && (message.RxIndex>=4))//信息是发给该从机站的
	{
		rt_sem_release(rs485_lock);
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
		rt_sem_take(rs485_lock,RT_WAITING_FOREVER);//永久等待信号量
		if(message.RxIndex>=4)
		{
			if(!crc_modbus(message.RxBuffer,message.RxIndex))
			{
				switch(message.RxBuffer[1])
				{
					case 0x01:
						Modbus_01H();
						break;
					case 0x04:
						Modbus_04H();
						break;
					default:
						message.Rsp = Error_Cmd;
						Send_ErrorAck(message.Rsp);
						break;
				}
				//rs485_write(message.RxBuffer,message.RxIndex);
			}
		}	
		
		rt_memset(&message,0,sizeof(message));//清空结构体
		rt_thread_delay(100);//线程延时
	}
}

int modbus_init()
{
	rt_pin_mode(RS485,PIN_MODE_OUTPUT);//485使能引脚
	rt_pin_write(RS485,PIN_LOW);
rt_pin_mode(90,PIN_MODE_OUTPUT);
rt_pin_write(90,PIN_HIGH);	
	rt_memset(&message,0,sizeof(message));//清空结构体
	
	rs485_lock = rt_sem_create("RS485",0,RT_IPC_FLAG_FIFO);//初始化互斥量
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

/***************modbus 自用功能函数***************/

/*
* 函数名称：rs485_write
* 函数功能：调用485接口，发送数据
* 输入参数：buffer --> 发送数据的头指针
* 			length --> 发送数据的字节长度
*/
static rt_err_t rs485_write(rt_uint8_t *buffer,rt_uint16_t length)
{
	rt_pin_write(RS485,PIN_HIGH);//发送使能
	rt_device_write(dev,0,buffer,length);
	rt_pin_write(RS485,PIN_LOW);//接收使能
	return RT_EOK;
}

/*
* 函数名称：crc_modbus
* 函数功能：计算 modbus 类型的 crc校验值
* 输入参数：buffer --> 进行校验的数据头指针
* 			size --> 校验数据的字节长度
*/
static rt_uint16_t crc_modbus(rt_uint8_t *buffer,rt_uint8_t size)
{
	rt_uint16_t crc = 0xFFFF;
	for(;size>0;size--)
	{
		crc ^= *buffer++;
		for(int i=0;i<8;i++)
		{
			if(crc & 0x0001)
			{
				crc = 0x7fff & (crc>>1);
				crc ^= 0xA001;
			}
			else
			{
				crc = 0x7fff & (crc>>1);
			}
		}
	}
	crc = crc>>8 | crc<<8;//crc 结果高低字节转换
	return crc;
}

/*
* 函数名称：Send_WithCrc
* 函数功能：发送带有 crc 校验的数据
*/
static void Send_WithCrc(rt_uint8_t *buffer,rt_uint8_t length)
{
	rt_uint8_t crc_value = 0x0000;
	crc_value = crc_modbus(buffer,length);//计算crc
	message.TxBuffer[message.TxIndex++] = crc_value>>8;//尾部追加crc校验码
	message.TxBuffer[message.TxIndex++] = crc_value;
	rs485_write(message.TxBuffer,message.TxIndex);//发送数据
}

/*
* 函数名称：Send_ErrorAck
* 函数功能：modbus 回发错误码
*/
static void Send_ErrorAck(rt_uint8_t ack)
{
	message.TxBuffer[message.TxIndex++] = message.RxBuffer[0];
	message.TxBuffer[message.TxIndex++] = message.RxBuffer[1] | 0x80;
	message.TxBuffer[message.TxIndex++] = ack;	
	Send_WithCrc(message.TxBuffer,message.TxIndex);//错误应答，回发三个字节
}

/*
* 函数名称：Modbus_01H
* 函数功能：Modbus 01H 功能码对应函数
*/
static void Modbus_01H(void)
{
	rt_pin_write(90,PIN_LOW);
}

/*
* 函数名称：Modbus_04H
* 函数功能：Modbus 04H 功能码对应函数
*/
static void Modbus_04H(void)
{
	rt_pin_write(90,PIN_HIGH);
}
