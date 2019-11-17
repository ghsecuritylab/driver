#ifndef __DRV_FRAM_H__
#define __DRV_FRAM_H__

#include <board.h>

/*
* 保护区域设置
* 提供给操作者使用
* 相关函数：fm25cl64_RDSR	fm25cl64_WRSR
* sample :  fm25cl64_WRSR(dev,FM25CL64_All);//存储区域全保护
*/
#define FM25CL64_None       0x00    //存储区不保护
#define FM25CL64_Quarter    0x04    //保护高四分之一地址 --> 1800h to 1FFFh
#define FM25CL64_Half       0x08    //保护高二分之一地址 --> 1000h to 1FFFh
#define FM25CL64_All        0x0C    //全保护             --> 0000h to 1FFFh

/*
* FM25CL64 操作命令码
* 驱动内部使用
*/
#define WREN    0x06    //写使能命令
#define WRDI    0x04    //写失能命令
#define RDSR    0x05    //读状态寄存器命令
#define WRSR    0x01    //写状态寄存器命令
#define READ    0x03    //读存储区命令
#define WRITE   0x02    //写存储区命令
struct fm25cl64
{
    struct rt_spi_device *parent;
    char *bus_name; // name of the SPI bus to attach
    char *device_name;
};
typedef struct fm25cl64 *fm25cl64_t;

rt_err_t fm25cl64_erase(fm25cl64_t device,rt_uint16_t start_addr,rt_uint16_t end_addr);
rt_err_t fm25cl64_RDSR(fm25cl64_t device,void *buffer);
rt_err_t fm25cl64_WRSR(fm25cl64_t device,void *buffer);
rt_err_t fm25cl64_MemoryRead(fm25cl64_t device,rt_uint16_t addr,void *data,rt_size_t length);
rt_err_t fm25cl64_MemoryWrite(fm25cl64_t device,rt_uint16_t addr,void *data,rt_size_t length);

rt_err_t fm25cl64_init(fm25cl64_t device,GPIO_TypeDef *cs_gpiox,uint16_t cs_gpio_pin);
void fm25cl64_deinit(fm25cl64_t dev);

#endif //drv_fram.h
