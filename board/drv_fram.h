#ifndef __DRV_FRAM_H__
#define __DRV_FRAM_H__

#include <board.h>

/*
* ������������
* �ṩ��������ʹ��
* ��غ�����fm25cl64_RDSR	fm25cl64_WRSR
* sample :  fm25cl64_WRSR(dev,FM25CL64_All);//�洢����ȫ����
*/
#define FM25CL64_None       0x00    //�洢��������
#define FM25CL64_Quarter    0x04    //�������ķ�֮һ��ַ --> 1800h to 1FFFh
#define FM25CL64_Half       0x08    //�����߶���֮һ��ַ --> 1000h to 1FFFh
#define FM25CL64_All        0x0C    //ȫ����             --> 0000h to 1FFFh

/*
* FM25CL64 ����������
* �����ڲ�ʹ��
*/
#define WREN    0x06    //дʹ������
#define WRDI    0x04    //дʧ������
#define RDSR    0x05    //��״̬�Ĵ�������
#define WRSR    0x01    //д״̬�Ĵ�������
#define READ    0x03    //���洢������
#define WRITE   0x02    //д�洢������
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
