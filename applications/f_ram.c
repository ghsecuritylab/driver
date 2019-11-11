#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <fm25cl64.h>
#include <center.h>

/****define in ai_sensor.c file****/
extern ad_sensor sensor_center[12];

static fm25cl64_t dev; 

/*
float value[48] = { 1.0	 ,0.0  ,1200.0 ,0.0 ,
					1.0	 ,0.0  ,1200.0 ,0.0 , 
					1.0	 ,0.0  ,1200.0 ,0.0 ,
					1.0	 ,0.0  ,1200.0 ,0.0 ,
					1.0	 ,0.0  ,1200.0 ,0.0 ,
					1.0	 ,0.0  ,1200.0 ,0.0 ,
					1.0	 ,0.0  ,1200.0 ,0.0 ,
					1.0	 ,0.0  ,1200.0 ,0.0 ,
					1.0	 ,0.0  ,1200.0 ,0.0 ,
					1.0	 ,0.0  ,1200.0 ,0.0 ,
					1.0	 ,0.0  ,1200.0 ,0.0 ,
					1.0	 ,0.0  ,1200.0 ,0.0 , };
*/
rt_uint32_t value[48] = { 105	 ,0  ,120000 ,0 ,
						  106	 ,0  ,120000 ,0 , 
						  107	 ,0  ,120000 ,0 ,
						  108	 ,0  ,120000 ,0 ,
						  109	 ,0  ,120000 ,0 ,
						  105	 ,0  ,120000 ,0 ,
						  105	 ,0  ,120000 ,0 ,
						  105	 ,0  ,120000 ,0 ,
						  108	 ,0  ,120000 ,0 ,
						  105	 ,0  ,120000 ,0 ,
						  105	 ,0  ,120000 ,0 ,
						  110	 ,0  ,120000 ,0 , };


/*
* reverse the byte order 
*/
static void byte_reverse(rt_uint32_t *data)
{
	int byte = 0;
	for(int i=3;i>=0;i--)
	{
		byte |= (*data&0x000000ff)<<(8*i);
		*data >>= 8;
	}
	*data = byte;
}

/*
* crc-32/MPEG-2 check
* first check : crc = 0xffffffff
*/
static rt_uint32_t crc32(rt_uint8_t *buffer,rt_uint8_t length)
{
	rt_uint32_t crc = 0xffffffff;
	for(int i=0;i<length;i++)
	{
		crc ^= (*buffer++ <<24);
		for (int j = 0; j < 8; j ++) 
		{
			if (crc & 0x80000000) 
			{
				crc = (crc<<1) ^ 0x04c11db7;				
			}
			else
			{
				crc <<= 1;
			}				
		}
		crc = crc ^ 0x00000000;
	}
	return crc;
}

/*
* 当 ADC 通道参数更改时调用
* 计算更改后的整体数据 CRC，并写入
* 重新读取并校验
*/
rt_err_t update_crc(void)
{
	rt_uint32_t buffer[49] = {0};

	fm25cl64_MemoryRead(dev,0x1800,buffer,192);//从 F_RAM 读取通道参数
	buffer[48] = crc32((rt_uint8_t *)buffer,192);//计算新数据的 crc
	
	fm25cl64_WRSR(dev,&fm_unprotect);//取消写保护
	byte_reverse((rt_uint32_t*)&buffer[48]);//反转 crc 校验码的高低位顺序（keil小端模式）
	fm25cl64_MemoryWrite(dev,CRC_VALUE,&buffer[48],4);//写入新的 crc 校验码
	fm25cl64_WRSR(dev,&fm_protect);//开启写保护
	
	fm25cl64_MemoryRead(dev,0x1800,buffer,196);//从 F_RAM 读取数据和校验码
	if(crc32((rt_uint8_t *)buffer,196)==0)//通道参数与校验码一起校验
	{
		rt_kprintf("F_RAM new value store success!\n");
		return RT_EOK;//校验成功
	}
	return RT_ERROR;//校验失败
}

/*
* 把默认定义的数据写入铁电
*/
static void default_write(void)
{	
	fm25cl64_WRSR(dev,&fm_unprotect);//取消写保护
	fm25cl64_MemoryWrite(dev,0x1800,value,192);//写入默认设置的参数，48个值
	fm25cl64_WRSR(dev,&fm_protect);//开启写保护
}

/*
* 将读取的数据按规则存入传感数组
*/
static rt_err_t data_set(rt_uint32_t *buffer)
{
	for(int i=0;i<12;i++)
	{
		sensor_center[i].radio = *buffer++/adc_point;
		sensor_center[i].offset = *buffer++/adc_point;
		sensor_center[i].full = *buffer++/adc_point;
		sensor_center[i].zero = *buffer++/adc_point;
	}
	return RT_EOK;
}

static void fm_entry(void *parameter)
{	
	rt_uint32_t buffer[49] = {0};//0~47存通道参数，48存crc校验值
		
	fm25cl64_MemoryRead(dev,0x1800,buffer,196);//从F_RAM读取数据
	
	if(crc32((rt_uint8_t*)buffer,196)!=0)//从F_RAM读取的数据校验失败
	{
		rt_uint32_t crc = 0;
		
		default_write();//写入所有传感器数据
		crc = crc32((rt_uint8_t*)value,192);//得到数据的CRC校验码
		fm25cl64_WRSR(dev,&fm_unprotect);//取消写保护
		byte_reverse((rt_uint32_t*)&crc);//反转CRC校验码的高低位顺序（keil小端模式）
		fm25cl64_MemoryWrite(dev,CRC_VALUE,&crc,4);//写入数据的CRC校验码
		fm25cl64_WRSR(dev,&fm_protect);//开启写保护
		
		fm25cl64_MemoryRead(dev,0x1800,buffer,196);//从F_RAM读取数据
		if(crc32((rt_uint8_t*)buffer,196)!=0)//新写入数据校验失败
		{
			rt_kprintf("FM data check error!\n");
		}
		else//新写入数据校验成功
		{
			rt_kprintf("FM data check ok!\n");
		}
	}

	data_set(buffer);//读取的数据分配给传感中心结构体数组
	
	while(1)
	{
		fm25cl64_WRSR(dev,&fm_unprotect);//取消写保护
		fm25cl64_MemoryWrite(dev,CRC_VALUE,buffer,4);//写入数据的CRC校验码
		fm25cl64_WRSR(dev,&fm_protect);//开启写保护
		
		rt_thread_delay(1000);
	}
}

int f_ram_init()
{
	rt_err_t resault  = RT_ERROR;
	
	dev = rt_calloc(1,sizeof(fm25cl64_t));
	
	dev->bus_name = "spi2";
	dev->cs_pin_num = GET_PIN(B,12);
	dev->device_name = "fm25cl64";
	
	resault = fm25cl64_init(dev,GPIOB,GPIO_PIN_12);
	if(resault==RT_ERROR)
	{
		rt_kprintf("FM device init failed!\n");
		rt_free(dev);
	}
	else
	{
		rt_kprintf("FM device init success!\n");
	}
	
	{
		rt_thread_t tid;
		tid = rt_thread_create("fm25cl64",fm_entry,RT_NULL,1024,10,20);
		if(tid!=RT_NULL)
			rt_thread_startup(tid);
		else
			rt_kprintf("fm25cl64 thread create failed!\n");
	}
	
	return resault;
}
INIT_PREV_EXPORT(f_ram_init);


