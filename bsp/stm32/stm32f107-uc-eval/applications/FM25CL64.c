#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <fm25cl64.h>
#include <drv_spi.h>

struct rt_spi_device *fm; 

rt_err_t fm25cl64_RDSR(void *buffer)//¶Á FM25CL64 ×´Ì¬¼Ä´æÆ÷
{
	rt_uint8_t cmd = RDSR;
	rt_spi_send_then_recv(fm,&cmd,1,buffer,1);
	return RT_EOK;
}

rt_err_t fm25cl64_WRSR(void *buffer)//Ð´ FM25CL64 ×´Ì¬¼Ä´æÆ÷
{
	rt_uint8_t cmd = WREN;
	rt_spi_send(fm,&cmd,1);
	cmd = WRSR;
	rt_spi_send_then_send(fm,&cmd,1,buffer,1);
	cmd = WRDI;
	rt_spi_send(fm,&cmd,1);
	return RT_EOK;
}

rt_err_t fm25cl64_MemoryRead(rt_uint16_t addr,rt_uint8_t *data,rt_size_t length)
{
	rt_uint8_t cmd[3] = {READ,addr>>8,addr&0x0f};
	rt_spi_send_then_recv(fm,cmd,3,data,length);
	return RT_EOK;
}

rt_err_t fm25cl64_MemoryWrite(rt_uint16_t addr,rt_uint8_t *data,rt_size_t length)
{
	rt_uint8_t cmd[5] = {WREN,WRITE,addr>>8,addr&0x0f,WRDI};
	rt_spi_send(fm,&cmd[0],1);
	rt_spi_send_then_send(fm,&cmd[1],3,data,length);
	rt_spi_send(fm,&cmd[4],1);
	return RT_EOK;
}


static void fm25cl64_entry(void *parameter)
{
	rt_uint8_t buffer[8] = {0};
	float t[2] = {12.3};
	
	while(1)
	{
		fm25cl64_RDSR(buffer);
		
		rt_kprintf("%d\n",buffer[0]);
		rt_thread_delay(1000);
	}
}

int fm25cl64_init()
{
	rt_pin_mode(GET_PIN(B,12),PIN_MODE_OUTPUT);
	rt_pin_write(GET_PIN(B,12),PIN_HIGH);
	
	rt_hw_spi_device_attach("spi2","fm25cl64",GPIOB,GPIO_PIN_12);
	
	fm = rt_calloc(1,sizeof(struct rt_spi_device));
	
	fm = (struct rt_spi_device *)rt_device_find("fm25cl64");
	if(fm==RT_NULL)
		rt_kprintf("fm25cl64 find error!\n");
	
	{
		struct rt_spi_configuration cfg;
		cfg.data_width = 8;
		cfg.max_hz = 18*1000*1000;
		cfg.mode = RT_SPI_MASTER|RT_SPI_MODE_0|RT_SPI_MSB;
		
		rt_spi_configure(fm,&cfg);
	}
	
	{
		rt_thread_t tid;
		tid = rt_thread_create("fm25cl64",fm25cl64_entry,RT_NULL,1024,8,20);
		if(tid != RT_NULL)
			rt_thread_startup(tid);
		else 
			rt_kprintf("fm25cl64 task create error!\n");
	}
	return 0;
}
INIT_DEVICE_EXPORT(fm25cl64_init);

void fm25cl64_deinit(struct rt_spi_device *dev)
{
	RT_ASSERT(dev!=RT_NULL);
	rt_free(dev);
}
