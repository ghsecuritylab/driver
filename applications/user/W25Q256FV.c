#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "w25q256fv.h"
#include "drv_spi.h"
#include "drv_log.h"
#define LOG_TAG	"W25Q256FV"

/*
* �������ƣ�W25Q256_RDSR
* �������ܣ���W25Q256��״̬�Ĵ���
* ����˵����dev �豸ָ�룻cmd ָ��
* �� �� ֵ����Ӧ�Ĵ�����ֵ
*/
static rt_uint8_t W25Q256_RDSR(W25Q256_t dev,rt_uint8_t cmd)
{
	rt_uint8_t res = -1;
	rt_spi_send_then_recv(dev->spi_device,&cmd,1,&res,1);
	return res;
}

/*
* �������ƣ�W25Q256_WRSR
* �������ܣ�дW25Q256��״̬�Ĵ���
* ����˵����dev �豸ָ�룻cmd ָ�value д��ֵ
* �� �� ֵ��ʧ�� 0���ɹ� д���ֽ���
*/
static void W25Q256_WRSR(W25Q256_t dev,rt_uint8_t cmd,rt_uint8_t value)
{
	rt_uint8_t ins = W25Q_WREN;
	rt_spi_send(dev->spi_device,&ins,1);//дʹ��
	ins = cmd;
	rt_spi_send_then_send(dev->spi_device,&ins,1,&value,1);
	ins = W25Q_WRDI;
	rt_spi_send(dev->spi_device,&ins,1);//дʧ��
}

/*
* �������ƣ�W25Q256_WaitBusy
* �������ܣ��ȴ��豸�˳�æ״̬
*/
static void W25Q256_WaitBusy(W25Q256_t dev)
{
	while(W25Q256_RDSR(dev,W25Q_RDSR1)&0x01);
}

/*
* �������ƣ�W25Q256_Protect
* �������ܣ������豸�ı�������
*/
void W25Q256_Protect(W25Q256_t dev,lock_area range)
{
	rt_uint8_t state = 0x00;
	state = W25Q256_RDSR(dev,W25Q_RDSR1);	//��ȡ�Ĵ�����ǰֵ
	state &= 0xD0;
	state |= range;	//ֻ�ı���������λ
	W25Q256_WRSR(dev,W25Q_WRSR1,state);	
}

/*
* �������ƣ�W25Q256_SectorWR
* �������ܣ��豸������д����
* ����������dev �豸ָ�룻index ��ʼ�����ţ�buffer ����ָ�룻length �����ֽڳ���
* �� �� ֵ��ռ��ҳ��
*/
static rt_size_t W25Q256_SectorWR(W25Q256_t dev,rt_uint16_t index,rt_uint8_t *buffer,rt_uint16_t length)
{
	rt_uint8_t send_buffer[5] = {W25Q_WREN};
	rt_uint32_t addr_start = index * 4096;//�������������ż�����ʼ������ַ
	rt_uint16_t used_num = 0;
	RT_ASSERT(index<Sector_Total);//��������С����������Ŀ
	
	rt_spi_send(dev->spi_device,&send_buffer[0],1);//дʹ��
	send_buffer[0] = W25Q_SEER;//4k����ָ��
	send_buffer[1] = (rt_uint8_t)(addr_start>>24);
	send_buffer[2] = (rt_uint8_t)(addr_start>>16);
	send_buffer[3] = (rt_uint8_t)(addr_start>>8);
	send_buffer[4] = (rt_uint8_t)(addr_start);
	rt_spi_send(dev->spi_device,send_buffer,5);//����ָ�����ʼ��ַ�����4K����
	W25Q256_WaitBusy(dev);
	
	for(int i=0;i<16;i++)//���дһ������
	{
		send_buffer[0] = W25Q_WREN;
		rt_spi_send(dev->spi_device,&send_buffer[0],1);//дʹ��
		send_buffer[0] = W25Q_WRPA;//ҳдָ��
		send_buffer[1] = (rt_uint8_t)(addr_start>>24);
		send_buffer[2] = (rt_uint8_t)(addr_start>>16);
		send_buffer[3] = (rt_uint8_t)(addr_start>>8);
		send_buffer[4] = (rt_uint8_t)(addr_start);
		rt_spi_send_then_send(dev->spi_device,send_buffer,5,buffer,length>256?256:length);
		W25Q256_WaitBusy(dev);//�ȴ��豸�������
		addr_start += 256;
		buffer += 256;
		used_num++;
		if(length>256)	//ѭ��һ�Σ�����һҳ���ֽ�
			length -= 256;
		else			//���һ��ûд��һҳ������
			break;
	}
	send_buffer[0] = W25Q_WRDI;
	rt_spi_send(dev->spi_device,&send_buffer[0],1);//дʧ��	
	return used_num;
}

/*
* �������ƣ�W25Q256_flash_read
* �������ܣ����豸��ȡ����
* ����˵����pos ��ʼ�����ţ�buffer �洢������ʼ��ַ��size �ֽ�����
* �� �� ֵ��ָ�����ֽ�����
*/
rt_size_t W25Q256_flash_read(W25Q256_t dev,rt_off_t pos,void* buffer,rt_size_t size)
{
	W25Q256_t flash = (struct W25Q256 *)dev;
	rt_uint8_t send_buffer[5] = {0};
	rt_uint32_t addr_start = pos * 4096;//�������������ż�����ʼ������ַ
	
	send_buffer[0] = W25Q_READ;
	send_buffer[1] = (rt_uint8_t)(addr_start>>24);
	send_buffer[2] = (rt_uint8_t)(addr_start>>16);
	send_buffer[3] = (rt_uint8_t)(addr_start>>8);
	send_buffer[4] = (rt_uint8_t)(addr_start);
	
	rt_spi_send_then_recv(flash->spi_device,send_buffer,5,buffer,size);
	
	return size;
}

/*
* �������ƣ�W25Q256_flash_write
* �������ܣ����豸д������
* ����˵����pos ������ʼ��ַ��buffer ������ʼ��ַ��size д�������ֽ���
* �� �� ֵ��ռ��������
*/
rt_size_t W25Q256_flash_write(W25Q256_t dev, rt_off_t pos,rt_uint8_t *buffer,rt_size_t size)
{
	rt_size_t res = pos;
	W25Q256_t flash = (struct W25Q256 *)dev;
	
	for(;;)
	{
		W25Q256_SectorWR(flash,pos,buffer,size>4096?4096:size);

		buffer += 4096;
		pos++;
		
		if(size>4096)
			size -= 4096;
		else
			break;
	}
	return (pos-res);
}

/*
* �������ƣ�W25Q256_init
* �������ܣ���ʼ���豸
* �� �� ֵ���ɹ�����RT_EOK;ʧ�ܷ���RT_FALSE
*/
rt_err_t W25Q256_init(W25Q256_t dev,const char *spi_bus,GPIO_TypeDef *gpiox,uint16_t gpio_pin)
{
	
	rt_hw_spi_device_attach(spi_bus,dev->name,gpiox,gpio_pin);
	
	dev->spi_device = (struct rt_spi_device*)rt_device_find(dev->name);
	if(dev->spi_device==RT_NULL)
	{
		LOG_E("W25Q256 device find failed!\n");
		return RT_ERROR;
	}
	
	/* 		SPI BUS Config		 */
	{
		struct rt_spi_configuration cfg;
		cfg.data_width = 8;
		cfg.mode = RT_SPI_MASTER|RT_SPI_MODE_3|RT_SPI_MSB;
		cfg.max_hz = 15 * 1000 * 1000;
		rt_spi_configure(dev->spi_device,&cfg);
	}
	
	/* 		Device Config		*/
	{
		rt_uint8_t init[3] = {W25Q_ENRE,W25Q_REDE,W25Q_EN4B};
		rt_spi_send(dev->spi_device,&init[0],1);//ʹ�ܸ�λ
		rt_spi_send(dev->spi_device,&init[1],1);//������λ
		rt_thread_delay(1);
		rt_spi_send(dev->spi_device,&init[2],1);//ʹ��32Bit��ַ
		W25Q256_WRSR(dev,W25Q_WRSR1,status1);
		W25Q256_WRSR(dev,W25Q_WRSR2,status2);
		W25Q256_WRSR(dev,W25Q_WRSR3,status3);
	}
	

	rt_uint8_t id[3] = {W25Q_JEID};
	rt_spi_send_then_recv(dev->spi_device,id,1,id,3);
	
	id[0] = W25Q256_RDSR(dev,W25Q_RDSR1);
	id[1] = W25Q256_RDSR(dev,W25Q_RDSR2);
	id[2] = W25Q256_RDSR(dev,W25Q_RDSR3);
	
	if((--id[2])!=status3)
	{
		LOG_E("W25Q256 Device Config Failed!\n");
		return RT_ERROR;
	}
	
	LOG_I("W25Q256 Device Config Success!\n");
	return RT_EOK;;
}

/*
* �������ƣ�W25Q256_Deinit
* �������ܣ��ͷ�������ڴ�
*/
void W25Q256_Deinit(W25Q256_t dev)
{
	RT_ASSERT(dev);
	rt_free(dev);// �ͷ��豸�ڴ�ռ�
}
