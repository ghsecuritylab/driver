#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <fm25cl64.h>
#include <drv_spi.h>

/*
* function Name : fm25cl64_erase
* description 	: erase fm25cl64 memory area
* 	@param	device		: point of the fm25cl64 device
* 	@param	start_addr	: start address of the memory to erase
* 	@param	end_addr	: end address of the memory to erase
* return 		: RT_EOK
*/
rt_err_t fm25cl64_erase(fm25cl64_t device,rt_uint16_t start_addr,rt_uint16_t end_addr)
{
	rt_uint8_t cmd[5] = {WREN,WRITE,start_addr>>8,start_addr,WRDI};
	rt_spi_send(device->parent,&cmd[0],1);
	rt_spi_send_then_send(device->parent,&cmd[1],3,RT_NULL,end_addr-start_addr);
	rt_spi_send(device->parent,&cmd[4],1);
	return RT_EOK;
}

/*
* function Name : fm25cl64_RDSR
* description 	: read fm25cl64 status register
* 	@param	device	: point of the fm25cl64 device
* 	@param	*buffer	: point of the store buffer
* return 		: RT_EOK
*/
rt_err_t fm25cl64_RDSR(fm25cl64_t device,void *buffer)//¶Á FM25CL64 ×´Ì¬¼Ä´æÆ÷
{
	rt_uint8_t cmd = RDSR;
	rt_spi_send_then_recv(device->parent,&cmd,1,buffer,1);
	return RT_EOK;
}

/*
* function Name : fm25cl64_WRSR
* description 	: write fm25cl64 status register
* 	@param	device	: point of the fm25cl64 device
* 	@param	*buffer	: point of the value buffer
* return 		: RT_EOK
*/
rt_err_t fm25cl64_WRSR(fm25cl64_t device,rt_uint8_t buffer)//Ð´ FM25CL64 ×´Ì¬¼Ä´æÆ÷
{
	rt_uint8_t cmd = WREN;
	rt_spi_send(device->parent,&cmd,1);
	cmd = WRSR;
	rt_spi_send_then_send(device->parent,&cmd,1,&buffer,1);
	cmd = WRDI;
	rt_spi_send(device->parent,&cmd,1);
	return RT_EOK;
}

/*
* function Name : fm25cl64_MemoryRead
* description 	: read fm25cl64 memory area data
* 	@param	device	: point of the fm25cl64 device
* 	@param	addr	: address of the memory area
* 	@param	*data	: point of the store buffer
* 	@param	length	: nbytes to read
* return 		: RT_EOK
*/
rt_err_t fm25cl64_MemoryRead(fm25cl64_t device,rt_uint16_t addr,void *data,rt_size_t length)
{
	rt_uint8_t cmd[3] = {READ,addr>>8,addr};
	rt_spi_send_then_recv(device->parent,cmd,3,data,length);
	return RT_EOK;
}

/*
* function Name : fm25cl64_MemoryWrite
* description 	: write fm25cl64 memory area data
* 	@param	device	: point of the fm25cl64 device
* 	@param	addr	: address of the memory area
* 	@param	*data	: point of the value write
* 	@param	length	: nbytes to write
* return 		: RT_EOK
*/
rt_err_t fm25cl64_MemoryWrite(fm25cl64_t device,rt_uint16_t addr,void *data,rt_size_t length)
{
	rt_uint8_t cmd[5] = {WREN,WRITE,addr>>8,addr,WRDI};
	rt_spi_send(device->parent,&cmd[0],1);
	rt_spi_send_then_send(device->parent,&cmd[1],3,data,length);
	rt_spi_send(device->parent,&cmd[4],1);
	return RT_EOK;
}

/*
* function Name : fm25cl64_init
* description 	: Initialize the F_RAM device 
*	@param	device	 	: point of fm25cl64 device
* 	@param	cs_gpiox 	: GPIO peripheral of the cs_pin
* 	@param	cs_gpio_pin	: pin number of the cs_pin
* return		: the resault of initialization
*/
rt_err_t fm25cl64_init(fm25cl64_t device,GPIO_TypeDef *cs_gpiox,uint16_t cs_gpio_pin)
{
	rt_pin_mode(device->cs_pin_num,PIN_MODE_OUTPUT);
	rt_pin_write(device->cs_pin_num,PIN_HIGH);
	
	rt_hw_spi_device_attach(device->bus_name,device->device_name,cs_gpiox,cs_gpio_pin);
	
	device->parent = (struct rt_spi_device *)rt_device_find(device->device_name);
	if(device->parent==RT_NULL)
	{
		rt_kprintf("%s find error!\n",device->device_name);
		return RT_ERROR;
	}
	
	/*******Config spi parameter********/
	{
		struct rt_spi_configuration cfg;
		cfg.data_width = 8;
		cfg.max_hz = 15*1000*1000;
		cfg.mode = RT_SPI_MASTER|RT_SPI_MODE_3|RT_SPI_MSB;
		
		rt_spi_configure(device->parent,&cfg);
	}
	
	return RT_EOK;
}

/*
* function Name : fm25cl64_deinit
* description 	: release the device memory
* parameter		: point of the device
* return		: none
*/
void fm25cl64_deinit(fm25cl64_t dev)
{
	RT_ASSERT(dev!=RT_NULL);
	rt_free(dev);
}
