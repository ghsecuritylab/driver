#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <modbus.h>

#define RS485	GET_PIN(G, 8)
static rt_device_t dev;
static rt_uint8_t buffer;

static rt_err_t modbus_data(rt_device_t dev,rt_size_t size)
{
	rt_device_read(dev,0,&buffer,1);
	rt_pin_write(104,PIN_HIGH);
	rt_device_write(dev,0,&buffer,1);
	rt_pin_write(104,PIN_LOW);
	return RT_EOK;
}

static void modbus_entry(void *parameter)
{
	rt_uint8_t data[20] = "alone";
	while(1)
	{
		rt_pin_write(RS485,PIN_HIGH);
		rt_device_write(dev,0,data,sizeof(data));
		rt_pin_write(RS485,PIN_LOW);
		rt_thread_delay(2000);
	}
}

int modbus_init()
{
	rt_pin_mode(RS485,PIN_MODE_OUTPUT);
	rt_pin_write(RS485,PIN_LOW);
	
	dev = rt_device_find("uart2");
	if(dev==RT_NULL)
	{
		rt_kprintf("RS485´®¿Ú²éÕÒÊ§°Ü");
		return RT_ERROR;
	}
	
	rt_device_open(dev,RT_DEVICE_FLAG_INT_RX);
	rt_device_set_rx_indicate(dev,modbus_data);
	
	{
		rt_thread_t tid;
		tid = rt_thread_create("modbus",modbus_entry,RT_NULL,1024,20,20);
		if(tid!=RT_NULL)
		{
			rt_thread_startup(tid);
		}
	}
	return RT_EOK;
}
INIT_PREV_EXPORT(modbus_init);
