#ifndef __FM25CL64_H__
#define __FM25CL64_H__

#include <board.h>

#define WREN	0x06	//Ğ´Ê¹ÄÜÃüÁî
#define WRDI	0x04	//Ğ´Ê§ÄÜÃüÁî
#define RDSR	0x05	//¶Á×´Ì¬¼Ä´æÆ÷ÃüÁî
#define WRSR	0x01	//Ğ´×´Ì¬¼Ä´æÆ÷ÃüÁî
#define READ	0x03	//¶Á´æ´¢ÇøÃüÁî
#define WRITE	0x02	//Ğ´´æ´¢ÇøÃüÁî



rt_err_t fm25cl64_RDSR(void *buffer);
rt_err_t fm25cl64_WRSR(void *buffer);
rt_err_t fm25cl64_MemoryRead(rt_uint16_t addr,rt_uint8_t *data,rt_size_t length);
rt_err_t fm25cl64_MemoryWrite(rt_uint16_t addr,rt_uint8_t *data,rt_size_t length);


void fm25cl64_deinit(struct rt_spi_device *dev);

#endif //fm25cl64.h
