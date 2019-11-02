#include <board.h>
#include <rtthread.h>
#include "w25q256fv.h"


/*
* �������ƣ�W25Q256_RDSR
* �������ܣ���W25Q256��״̬�Ĵ���
* ����˵����dev �豸ָ�룻cmd ָ��
* �� �� ֵ����Ӧ�Ĵ�����ֵ
*/
static rt_uint8_t W25Q256_RDSR(W25Q256_t dev,rt_uint8_t cmd)
{
	return rt_spi_sendrecv8(dev->flash_device,cmd);
}

/*
* �������ƣ�W25Q256_WRSR
* �������ܣ�дW25Q256��״̬�Ĵ���
* ����˵����dev �豸ָ�룻cmd ָ�value д��ֵ
* �� �� ֵ��ʧ�� 0���ɹ� д���ֽ���
*/
static rt_size_t W25Q256_WRSR(W25Q256_t dev,rt_uint8_t cmd,rt_uint8_t value)
{
	rt_size_t res = 0;
	rt_spi_sendrecv8(dev->flash_device,W25Q_WREN);//дʹ��
	res = rt_spi_send_then_send(dev->flash_device,&cmd,1,&value,1);
	rt_spi_sendrecv8(dev->flash_device,W25Q_WRDI);//дʧ��
	return res;
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
* �� �� ֵ��д���ֽ���
*/
static rt_size_t W25Q256_Protect(W25Q256_t dev,lock_area range)
{
	rt_uint8_t state = 0x00;
	state = W25Q256_RDSR(dev,W25Q_RDSR1);	//��ȡ�Ĵ�����ǰֵ
	state &= range;	//ֻ�ı���������λ
	return W25Q256_WRSR(dev,W25Q_WRSR1,state);	
}

/*
* �������ƣ�W25Q256_SectorWR
* �������ܣ��豸������д����
* ����������dev �豸ָ�룻index ��ʼ�����ţ�buffer ����ָ�룻length �����ֽڳ���
* �� �� ֵ��ռ��ҳ��
*/
static rt_size_t W25Q256_SectorWR(W25Q256_t dev,rt_uint16_t index,rt_uint8_t *buffer,rt_uint16_t length)
{
	rt_uint8_t send_buffer[5] = {0};
	rt_uint32_t addr_start = index * 4096;//�������������ż�����ʼ������ַ
	rt_uint16_t used_num = 0;
	RT_ASSERT(index<Sector_Total);//��������С����������Ŀ
	
	rt_spi_sendrecv8(dev->flash_device,W25Q_WREN);//дʹ��
	send_buffer[0] = W25Q_SEER;//4k����ָ��
	send_buffer[1] = (rt_uint8_t)(addr_start>>24);
	send_buffer[2] = (rt_uint8_t)(addr_start>>16);
	send_buffer[3] = (rt_uint8_t)(addr_start>>8);
	send_buffer[4] = (rt_uint8_t)(addr_start);
	rt_spi_send(dev->flash_device,send_buffer,5);//����ָ�����ʼ��ַ�����4K����
	W25Q256_WaitBusy(dev);
	
	for(int i=0;i<16;i++)//���дһ������
	{
		rt_spi_sendrecv8(dev->flash_device,W25Q_WREN);//дʹ��
		send_buffer[0] = W25Q_WRPA;//ҳдָ��
		send_buffer[1] = (rt_uint8_t)(addr_start>>24);
		send_buffer[2] = (rt_uint8_t)(addr_start>>16);
		send_buffer[3] = (rt_uint8_t)(addr_start>>8);
		send_buffer[4] = (rt_uint8_t)(addr_start);
		rt_spi_send_then_send(dev->flash_device,send_buffer,5,buffer,length>256?256:length);
		W25Q256_WaitBusy(dev);//�ȴ��豸�������
		addr_start += 256;
		buffer += 256;
		used_num++;
		if(length>256)	//ѭ��һ�Σ�����һҳ���ֽ�
			length -= 256;
		else			//���һ��ûд��һҳ������
			break;
	}
	
	rt_spi_sendrecv8(dev->flash_device,W25Q_WRDI);//дʧ��	
	return used_num;
}

/*
* �������ƣ�W25Q256_flash_read
* �������ܣ����豸��ȡ����
* ����˵����pos �����ţ�buffer �洢����size �ֽ�����
* �� �� ֵ��
*/
static rt_size_t W25Q256_flash_read(rt_device_t dev,rt_off_t pos,void* buffer,rt_size_t size)
{
	W25Q256_t flash = (struct W25Q256 *)dev;
	
	
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
