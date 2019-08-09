#ifndef __FM25CL64_H__
#define __FM25CL64_H__

#include <board.h>

/*****command of fm25cl64 ops******/
#define WREN	0x06	//Ğ´Ê¹ÄÜÃüÁî
#define WRDI	0x04	//Ğ´Ê§ÄÜÃüÁî
#define RDSR	0x05	//¶Á×´Ì¬¼Ä´æÆ÷ÃüÁî
#define WRSR	0x01	//Ğ´×´Ì¬¼Ä´æÆ÷ÃüÁî
#define READ	0x03	//¶Á´æ´¢ÇøÃüÁî
#define WRITE	0x02	//Ğ´´æ´¢ÇøÃüÁî

struct fm25cl64
{
	struct rt_spi_device *parent;
	char *bus_name;		// name of the SPI bus to attach
	char *device_name;
	rt_uint8_t cs_pin_num;
};
typedef struct fm25cl64 *fm25cl64_t;


rt_err_t fm25cl64_erase(fm25cl64_t device,rt_uint16_t start_addr,rt_uint16_t end_addr);
rt_err_t fm25cl64_RDSR(fm25cl64_t device,void *buffer);
rt_err_t fm25cl64_WRSR(fm25cl64_t device,void *buffer);
rt_err_t fm25cl64_MemoryRead(fm25cl64_t device,rt_uint16_t addr,void *data,rt_size_t length);
rt_err_t fm25cl64_MemoryWrite(fm25cl64_t device,rt_uint16_t addr,void *data,rt_size_t length);

rt_err_t fm25cl64_init(fm25cl64_t device,GPIO_TypeDef *cs_gpiox,uint16_t cs_gpio_pin);
void fm25cl64_deinit(fm25cl64_t dev);

#endif //fm25cl64.h
