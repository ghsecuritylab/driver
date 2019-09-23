#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <modbus.h>

#define RS485	GET_PIN(G, 8)
static rt_device_t dev;//�����豸
static struct MODBUS_timer RS485_timer;//��ʱ���ṹ��
static rt_sem_t rs485_lock = RT_NULL;//MODBUS Э����
static rs485_t message;//RS485 ����

/*	���ڽ��ջص�����	*/
static rt_err_t modbus_data(rt_device_t dev,rt_size_t size)
{
	if(message.RxIndex>=Message_MAX)
	{
		message.Rsp = Error_Value;
		message.RxIndex = Message_MAX-1;
	}
	rt_device_read(dev,0,&message.RxBuffer[message.RxIndex++],1);//����һ���ַ�
	rt_device_write(RS485_timer.dev_timer,0,&RS485_timer.timeout,sizeof(RS485_timer.timeout));//������ʱ
	return RT_EOK;
}

/*	��ʱ����ʱ�ص�����	*/
static rt_err_t timer_out(rt_device_t dev,rt_size_t size)
{
	if((message.RxBuffer[0] == SLAVE) && (message.RxIndex>=4))//��Ϣ�Ƿ����ôӻ�վ��
	{
		rt_sem_release(rs485_lock);
		rt_kprintf("�ӻ�����\n");
	}
	else//��Ϣ���Ƿ����ôӻ�վ��
	{
		rt_kprintf("�ӻ��ܾ�\n");
		rt_memset(&message,0,sizeof(message));//��սṹ��
	}
	rt_kprintf("��ʱ����ʱ!\n");
	return RT_EOK;
}

/*	modbus ��������	*/
static void modbus_app(void *parameter)
{
	while(1)
	{
		rt_sem_take(rs485_lock,RT_WAITING_FOREVER);//���õȴ��ź���
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
		
		rt_memset(&message,0,sizeof(message));//��սṹ��
		rt_thread_delay(100);//�߳���ʱ
	}
}

int modbus_init()
{
	rt_pin_mode(RS485,PIN_MODE_OUTPUT);//485ʹ������
	rt_pin_write(RS485,PIN_LOW);
rt_pin_mode(90,PIN_MODE_OUTPUT);
rt_pin_write(90,PIN_HIGH);	
	rt_memset(&message,0,sizeof(message));//��սṹ��
	
	rs485_lock = rt_sem_create("RS485",0,RT_IPC_FLAG_FIFO);//��ʼ��������
	if(rs485_lock == RT_NULL)
	{
		rt_kprintf("modbus mutex create error!\n");
		return RT_ERROR;
	}
	
	/* �����豸���� */
	dev = rt_device_find("uart2");
	if(dev==RT_NULL)
	{
		rt_kprintf("RS485���ڲ���ʧ��!\n");
		return RT_ERROR;
	}
	rt_device_open(dev,RT_DEVICE_FLAG_INT_RX);
	rt_device_set_rx_indicate(dev,modbus_data);//���ڽ��ջص�����
	
	/* ��ʱ���豸���� */
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
	rt_device_set_rx_indicate(RS485_timer.dev_timer,timer_out);//��ʱ����ʱ�ص�����
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

/***************modbus ���ù��ܺ���***************/

/*
* �������ƣ�rs485_write
* �������ܣ�����485�ӿڣ���������
* ���������buffer --> �������ݵ�ͷָ��
* 			length --> �������ݵ��ֽڳ���
*/
static rt_err_t rs485_write(rt_uint8_t *buffer,rt_uint16_t length)
{
	rt_pin_write(RS485,PIN_HIGH);//����ʹ��
	rt_device_write(dev,0,buffer,length);
	rt_pin_write(RS485,PIN_LOW);//����ʹ��
	return RT_EOK;
}

/*
* �������ƣ�crc_modbus
* �������ܣ����� modbus ���͵� crcУ��ֵ
* ���������buffer --> ����У�������ͷָ��
* 			size --> У�����ݵ��ֽڳ���
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
	crc = crc>>8 | crc<<8;//crc ����ߵ��ֽ�ת��
	return crc;
}

/*
* �������ƣ�Send_WithCrc
* �������ܣ����ʹ��� crc У�������
*/
static void Send_WithCrc(rt_uint8_t *buffer,rt_uint8_t length)
{
	rt_uint8_t crc_value = 0x0000;
	crc_value = crc_modbus(buffer,length);//����crc
	message.TxBuffer[message.TxIndex++] = crc_value>>8;//β��׷��crcУ����
	message.TxBuffer[message.TxIndex++] = crc_value;
	rs485_write(message.TxBuffer,message.TxIndex);//��������
}

/*
* �������ƣ�Send_ErrorAck
* �������ܣ�modbus �ط�������
*/
static void Send_ErrorAck(rt_uint8_t ack)
{
	message.TxBuffer[message.TxIndex++] = message.RxBuffer[0];
	message.TxBuffer[message.TxIndex++] = message.RxBuffer[1] | 0x80;
	message.TxBuffer[message.TxIndex++] = ack;	
	Send_WithCrc(message.TxBuffer,message.TxIndex);//����Ӧ�𣬻ط������ֽ�
}

/*
* �������ƣ�Modbus_01H
* �������ܣ�Modbus 01H �������Ӧ����
*/
static void Modbus_01H(void)
{
	rt_pin_write(90,PIN_LOW);
}

/*
* �������ƣ�Modbus_04H
* �������ܣ�Modbus 04H �������Ӧ����
*/
static void Modbus_04H(void)
{
	rt_pin_write(90,PIN_HIGH);
}
