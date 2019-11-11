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
* �� ADC ͨ����������ʱ����
* ������ĺ���������� CRC����д��
* ���¶�ȡ��У��
*/
rt_err_t update_crc(void)
{
	rt_uint32_t buffer[49] = {0};

	fm25cl64_MemoryRead(dev,0x1800,buffer,192);//�� F_RAM ��ȡͨ������
	buffer[48] = crc32((rt_uint8_t *)buffer,192);//���������ݵ� crc
	
	fm25cl64_WRSR(dev,&fm_unprotect);//ȡ��д����
	byte_reverse((rt_uint32_t*)&buffer[48]);//��ת crc У����ĸߵ�λ˳��keilС��ģʽ��
	fm25cl64_MemoryWrite(dev,CRC_VALUE,&buffer[48],4);//д���µ� crc У����
	fm25cl64_WRSR(dev,&fm_protect);//����д����
	
	fm25cl64_MemoryRead(dev,0x1800,buffer,196);//�� F_RAM ��ȡ���ݺ�У����
	if(crc32((rt_uint8_t *)buffer,196)==0)//ͨ��������У����һ��У��
	{
		rt_kprintf("F_RAM new value store success!\n");
		return RT_EOK;//У��ɹ�
	}
	return RT_ERROR;//У��ʧ��
}

/*
* ��Ĭ�϶��������д������
*/
static void default_write(void)
{	
	fm25cl64_WRSR(dev,&fm_unprotect);//ȡ��д����
	fm25cl64_MemoryWrite(dev,0x1800,value,192);//д��Ĭ�����õĲ�����48��ֵ
	fm25cl64_WRSR(dev,&fm_protect);//����д����
}

/*
* ����ȡ�����ݰ�������봫������
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
	rt_uint32_t buffer[49] = {0};//0~47��ͨ��������48��crcУ��ֵ
		
	fm25cl64_MemoryRead(dev,0x1800,buffer,196);//��F_RAM��ȡ����
	
	if(crc32((rt_uint8_t*)buffer,196)!=0)//��F_RAM��ȡ������У��ʧ��
	{
		rt_uint32_t crc = 0;
		
		default_write();//д�����д���������
		crc = crc32((rt_uint8_t*)value,192);//�õ����ݵ�CRCУ����
		fm25cl64_WRSR(dev,&fm_unprotect);//ȡ��д����
		byte_reverse((rt_uint32_t*)&crc);//��תCRCУ����ĸߵ�λ˳��keilС��ģʽ��
		fm25cl64_MemoryWrite(dev,CRC_VALUE,&crc,4);//д�����ݵ�CRCУ����
		fm25cl64_WRSR(dev,&fm_protect);//����д����
		
		fm25cl64_MemoryRead(dev,0x1800,buffer,196);//��F_RAM��ȡ����
		if(crc32((rt_uint8_t*)buffer,196)!=0)//��д������У��ʧ��
		{
			rt_kprintf("FM data check error!\n");
		}
		else//��д������У��ɹ�
		{
			rt_kprintf("FM data check ok!\n");
		}
	}

	data_set(buffer);//��ȡ�����ݷ�����������Ľṹ������
	
	while(1)
	{
		fm25cl64_WRSR(dev,&fm_unprotect);//ȡ��д����
		fm25cl64_MemoryWrite(dev,CRC_VALUE,buffer,4);//д�����ݵ�CRCУ����
		fm25cl64_WRSR(dev,&fm_protect);//����д����
		
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


