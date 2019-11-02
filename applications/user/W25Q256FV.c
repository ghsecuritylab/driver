#include <board.h>
#include <rtthread.h>
#include "w25q256fv.h"


/*
* 函数名称：W25Q256_RDSR
* 函数功能：读W25Q256的状态寄存器
* 参数说明：dev 设备指针；cmd 指令
* 返 回 值：对应寄存器的值
*/
static rt_uint8_t W25Q256_RDSR(W25Q256_t dev,rt_uint8_t cmd)
{
	return rt_spi_sendrecv8(dev->flash_device,cmd);
}

/*
* 函数名称：W25Q256_WRSR
* 函数功能：写W25Q256的状态寄存器
* 参数说明：dev 设备指针；cmd 指令；value 写入值
* 返 回 值：失败 0；成功 写入字节数
*/
static rt_size_t W25Q256_WRSR(W25Q256_t dev,rt_uint8_t cmd,rt_uint8_t value)
{
	rt_size_t res = 0;
	rt_spi_sendrecv8(dev->flash_device,W25Q_WREN);//写使能
	res = rt_spi_send_then_send(dev->flash_device,&cmd,1,&value,1);
	rt_spi_sendrecv8(dev->flash_device,W25Q_WRDI);//写失能
	return res;
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
* 返 回 值：写入字节数
*/
static rt_size_t W25Q256_Protect(W25Q256_t dev,lock_area range)
{
	rt_uint8_t state = 0x00;
	state = W25Q256_RDSR(dev,W25Q_RDSR1);	//读取寄存器当前值
	state &= range;	//只改变区域设置位
	return W25Q256_WRSR(dev,W25Q_WRSR1,state);	
}

/*
* 函数名称：W25Q256_SectorWR
* 函数功能：设备的扇区写操作
* 函数参数：dev 设备指针；index 起始扇区号；buffer 数据指针；length 数据字节长度
* 返 回 值：占用页数
*/
static rt_size_t W25Q256_SectorWR(W25Q256_t dev,rt_uint16_t index,rt_uint8_t *buffer,rt_uint16_t length)
{
	rt_uint8_t send_buffer[5] = {0};
	rt_uint32_t addr_start = index * 4096;//根据扇区索引号计算起始扇区地址
	rt_uint16_t used_num = 0;
	RT_ASSERT(index<Sector_Total);//扇区索引小于总扇区数目
	
	rt_spi_sendrecv8(dev->flash_device,W25Q_WREN);//写使能
	send_buffer[0] = W25Q_SEER;//4k擦除指令
	send_buffer[1] = (rt_uint8_t)(addr_start>>24);
	send_buffer[2] = (rt_uint8_t)(addr_start>>16);
	send_buffer[3] = (rt_uint8_t)(addr_start>>8);
	send_buffer[4] = (rt_uint8_t)(addr_start);
	rt_spi_send(dev->flash_device,send_buffer,5);//发送指令和起始地址，清除4K区域
	W25Q256_WaitBusy(dev);
	
	for(int i=0;i<16;i++)//最多写一个扇区
	{
		rt_spi_sendrecv8(dev->flash_device,W25Q_WREN);//写使能
		send_buffer[0] = W25Q_WRPA;//页写指令
		send_buffer[1] = (rt_uint8_t)(addr_start>>24);
		send_buffer[2] = (rt_uint8_t)(addr_start>>16);
		send_buffer[3] = (rt_uint8_t)(addr_start>>8);
		send_buffer[4] = (rt_uint8_t)(addr_start);
		rt_spi_send_then_send(dev->flash_device,send_buffer,5,buffer,length>256?256:length);
		W25Q256_WaitBusy(dev);//等待设备操作完成
		addr_start += 256;
		buffer += 256;
		used_num++;
		if(length>256)	//循环一次，减少一页的字节
			length -= 256;
		else			//最后一次没写够一页，跳出
			break;
	}
	
	rt_spi_sendrecv8(dev->flash_device,W25Q_WRDI);//写失能	
	return used_num;
}

/*
* 函数名称：W25Q256_flash_read
* 函数功能：从设备读取数据
* 参数说明：pos 扇区号；buffer 存储区域；size 字节数量
* 返 回 值：
*/
static rt_size_t W25Q256_flash_read(rt_device_t dev,rt_off_t pos,void* buffer,rt_size_t size)
{
	W25Q256_t flash = (struct W25Q256 *)dev;
	
	
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
