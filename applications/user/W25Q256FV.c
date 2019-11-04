#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "w25q256fv.h"
#include "drv_spi.h"
#include "drv_log.h"
#define LOG_TAG	"W25Q256FV"

/*
* 函数名称：W25Q256_RDSR
* 函数功能：读W25Q256的状态寄存器
* 参数说明：dev 设备指针；cmd 指令
* 返 回 值：对应寄存器的值
*/
static rt_uint8_t W25Q256_RDSR(W25Q256_t dev,rt_uint8_t cmd)
{
	rt_uint8_t res = -1;
	rt_spi_send_then_recv(dev->spi_device,&cmd,1,&res,1);
	return res;
}

/*
* 函数名称：W25Q256_WRSR
* 函数功能：写W25Q256的状态寄存器
* 参数说明：dev 设备指针；cmd 指令；value 写入值
* 返 回 值：失败 0；成功 写入字节数
*/
static void W25Q256_WRSR(W25Q256_t dev,rt_uint8_t cmd,rt_uint8_t value)
{
	rt_uint8_t ins = W25Q_WREN;
	rt_spi_send(dev->spi_device,&ins,1);//写使能
	ins = cmd;
	rt_spi_send_then_send(dev->spi_device,&ins,1,&value,1);
	ins = W25Q_WRDI;
	rt_spi_send(dev->spi_device,&ins,1);//写失能
}

/*
* 函数名称：W25Q256_WaitBusy
* 函数功能：等待设备退出忙状态
*/
static void W25Q256_WaitBusy(W25Q256_t dev)
{
	while(W25Q256_RDSR(dev,W25Q_RDSR1)&0x01);
}

/*
* 函数名称：W25Q256_Protect
* 函数功能：设置设备的保护区域
*/
void W25Q256_Protect(W25Q256_t dev,lock_area range)
{
	rt_uint8_t state = 0x00;
	state = W25Q256_RDSR(dev,W25Q_RDSR1);	//读取寄存器当前值
	state &= 0xD0;
	state |= range;	//只改变区域设置位
	W25Q256_WRSR(dev,W25Q_WRSR1,state);	
}

/*
* 函数名称：W25Q256_SectorWR
* 函数功能：设备的扇区写操作
* 函数参数：dev 设备指针；index 起始扇区号；buffer 数据指针；length 数据字节长度
* 返 回 值：占用页数
*/
static rt_size_t W25Q256_SectorWR(W25Q256_t dev,rt_uint16_t index,rt_uint8_t *buffer,rt_uint16_t length)
{
	rt_uint8_t send_buffer[5] = {W25Q_WREN};
	rt_uint32_t addr_start = index * 4096;//根据扇区索引号计算起始扇区地址
	rt_uint16_t used_num = 0;
	RT_ASSERT(index<Sector_Total);//扇区索引小于总扇区数目
	
	rt_spi_send(dev->spi_device,&send_buffer[0],1);//写使能
	send_buffer[0] = W25Q_SEER;//4k擦除指令
	send_buffer[1] = (rt_uint8_t)(addr_start>>24);
	send_buffer[2] = (rt_uint8_t)(addr_start>>16);
	send_buffer[3] = (rt_uint8_t)(addr_start>>8);
	send_buffer[4] = (rt_uint8_t)(addr_start);
	rt_spi_send(dev->spi_device,send_buffer,5);//发送指令和起始地址，清除4K区域
	W25Q256_WaitBusy(dev);
	
	for(int i=0;i<16;i++)//最多写一个扇区
	{
		send_buffer[0] = W25Q_WREN;
		rt_spi_send(dev->spi_device,&send_buffer[0],1);//写使能
		send_buffer[0] = W25Q_WRPA;//页写指令
		send_buffer[1] = (rt_uint8_t)(addr_start>>24);
		send_buffer[2] = (rt_uint8_t)(addr_start>>16);
		send_buffer[3] = (rt_uint8_t)(addr_start>>8);
		send_buffer[4] = (rt_uint8_t)(addr_start);
		rt_spi_send_then_send(dev->spi_device,send_buffer,5,buffer,length>256?256:length);
		W25Q256_WaitBusy(dev);//等待设备操作完成
		addr_start += 256;
		buffer += 256;
		used_num++;
		if(length>256)	//循环一次，减少一页的字节
			length -= 256;
		else			//最后一次没写够一页，跳出
			break;
	}
	send_buffer[0] = W25Q_WRDI;
	rt_spi_send(dev->spi_device,&send_buffer[0],1);//写失能	
	return used_num;
}

/*
* 函数名称：W25Q256_flash_read
* 函数功能：从设备读取数据
* 参数说明：pos 起始扇区号；buffer 存储区域起始地址；size 字节数量
* 返 回 值：指定的字节数量
*/
rt_size_t W25Q256_flash_read(W25Q256_t dev,rt_off_t pos,void* buffer,rt_size_t size)
{
	W25Q256_t flash = (struct W25Q256 *)dev;
	rt_uint8_t send_buffer[5] = {0};
	rt_uint32_t addr_start = pos * 4096;//根据扇区索引号计算起始扇区地址
	
	send_buffer[0] = W25Q_READ;
	send_buffer[1] = (rt_uint8_t)(addr_start>>24);
	send_buffer[2] = (rt_uint8_t)(addr_start>>16);
	send_buffer[3] = (rt_uint8_t)(addr_start>>8);
	send_buffer[4] = (rt_uint8_t)(addr_start);
	
	rt_spi_send_then_recv(flash->spi_device,send_buffer,5,buffer,size);
	
	return size;
}

/*
* 函数名称：W25Q256_flash_write
* 函数功能：向设备写入数据
* 参数说明：pos 扇区起始地址；buffer 数据起始地址；size 写入数据字节数
* 返 回 值：占用扇区数
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
* 函数名称：W25Q256_init
* 函数功能：初始化设备
* 返 回 值：成功返回RT_EOK;失败返回RT_FALSE
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
		rt_spi_send(dev->spi_device,&init[0],1);//使能复位
		rt_spi_send(dev->spi_device,&init[1],1);//启动复位
		rt_thread_delay(1);
		rt_spi_send(dev->spi_device,&init[2],1);//使用32Bit地址
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
* 函数名称：W25Q256_Deinit
* 函数功能：释放申请的内存
*/
void W25Q256_Deinit(W25Q256_t dev)
{
	RT_ASSERT(dev);
	rt_free(dev);// 释放设备内存空间
}
