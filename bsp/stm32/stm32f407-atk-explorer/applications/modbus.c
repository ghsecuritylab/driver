#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <modbus.h>

#define RS485	GET_PIN(G, 8)
static rt_device_t dev;//�����豸
static rt_device_t dev_tim;//��ʱ���豸
static rt_mutex_t rs485_lock;//MODBUS Э����
static rs485_t message;//RS485 ����

static rt_err_t modbus_data(rt_device_t dev,rt_size_t size)
{
	if(message.index>=1024)
	{
		message.report=1;
		return RT_FALSE; 
	}
	rt_device_read(dev,0,&message.buffer[message.index++],1);//����һ���ַ�
		
	return RT_EOK;
}

int modbus_init()
{
	rt_pin_mode(RS485,PIN_MODE_OUTPUT);
	rt_pin_write(RS485,PIN_LOW);
	
	rt_mutex_init(rs485_lock,"RS485",RT_IPC_FLAG_FIFO);//��ʼ��������
	
	/* �����豸���� */
	dev = rt_device_find("uart2");
	if(dev==RT_NULL)
	{
		rt_kprintf("RS485���ڲ���ʧ��");
		return RT_ERROR;
	}
	rt_device_open(dev,RT_DEVICE_FLAG_INT_RX);
	rt_device_set_rx_indicate(dev,modbus_data);
	
	/* ��ʱ���豸���� */
	dev_tim = rt_device_find("timer13");
	if(dev_tim==RT_NULL)
	{
		rt_kprintf("timer13 find error!\n");
		return RT_ERROR;
	}
	
	
	return RT_EOK;
}
INIT_PREV_EXPORT(modbus_init);



static rt_err_t rs485_write(rt_uint8_t *buffer,rt_uint16_t length)
{
	rt_pin_write(RS485,PIN_HIGH);
	rt_device_write(dev,0,buffer,sizeof(buffer));
	rt_pin_write(RS485,PIN_LOW);
	return RT_EOK;
}
