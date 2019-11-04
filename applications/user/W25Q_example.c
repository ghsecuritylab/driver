#include <board.h>
#include "W25Q256FV.h"

static W25Q256_t dev;




void W25Q_init()
{
	dev = rt_calloc(1,sizeof(struct W25Q256));
	if(dev==RT_NULL)
	{
		rt_kprintf("memory for W25Q256 is null!\n");
		return;
	}
	dev->gpiox = GPIOE;
	dev->gpio_pin = 4;
	dev->name = "W25Q256";
	if(W25Q256_init(dev,"spi4")!=RT_EOK)
		rt_kprintf("W25Q init falied!\n");
	else
		rt_kprintf("W25Q init success!\n");
	
	W25Q256_Deinit(dev);
}
MSH_CMD_EXPORT(W25Q_init,flash device init);
