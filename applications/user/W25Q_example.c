#include <board.h>
#include "W25Q256FV.h"
#include <drv_spi.h>
static W25Q256_t dev;
static rt_uint8_t flash[32] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
static rt_uint8_t recv[32] = {0};

void page()
{
	rt_uint8_t cmd[7] = {W25Q_WREN,W25Q_WRPA,0,0,0,0,W25Q_WRDI};
	rt_uint8_t buffer[32] = {0};
	rt_spi_send(dev->spi_device,cmd,1);
	rt_spi_send_then_send(dev->spi_device,&cmd[1],5,buffer,32);
	rt_spi_send(dev->spi_device,&cmd[6],1);
}
MSH_CMD_EXPORT(page,TEST page);

void flash_write()
{
	rt_size_t res=0;
	res = W25Q256_flash_write(dev,0,flash,32);
}
MSH_CMD_EXPORT(flash_write,write test);
	
void flash_read()
{
	rt_size_t res = 0;
	res = W25Q256_flash_read(dev,0,recv,32);
}
MSH_CMD_EXPORT(flash_read,read test);

void W25Q_init()
{
	dev = rt_calloc(1,sizeof(struct W25Q256));
	if(dev==RT_NULL)
	{
		rt_kprintf("memory for W25Q256 is null!\n");
		return;
	}
	dev->name = "W25Q256";
	if(W25Q256_init(dev,"spi4",GPIOE,GPIO_PIN_4)!=RT_EOK)
		rt_kprintf("W25Q init falied!\n");
	else
		rt_kprintf("W25Q init success!\n");
	
//	W25Q256_Deinit(dev);
}
MSH_CMD_EXPORT(W25Q_init,flash device init);
