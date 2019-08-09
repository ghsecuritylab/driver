#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <fm25cl64.h>
#include <center.h>

/****define in ai_sensor.c file****/
extern ad_sensor sensor_center[12];

static fm25cl64_t dev; 

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
static rt_uint32_t crc32(rt_uint8_t *buffer,rt_uint8_t length,rt_uint32_t crc)
{
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
* 
*/
rt_err_t update_crc(void)
{
	float buffer[48];
	rt_uint32_t new_crc,crc_test = 0;
	
	fm25cl64_MemoryRead(dev,0x1800,buffer,192);//��F_RAM��ȡ����
	new_crc = crc32((rt_uint8_t *)buffer,192,0xffffffff);//���������ݵ� crc
	crc_test = new_crc;
	
	fm25cl64_WRSR(dev,&fm_unprotect);//ȡ��д����
	byte_reverse((rt_uint32_t*)&new_crc);//��תcrcУ����ĸߵ�λ˳��keilС��ģʽ��
	fm25cl64_MemoryWrite(dev,CRC_VALUE,&new_crc,4);//д�����ݵ�CRCУ����
	fm25cl64_WRSR(dev,&fm_protect);//����д����
	
	fm25cl64_MemoryRead(dev,CRC_VALUE,&new_crc,4);//��ȡCRC����У��
	if(!crc32((rt_uint8_t*)&new_crc,4,crc_test))
	{
		return RT_EOK;//У��ɹ�
	}
	return RT_ERROR;//У��ʧ��
}

/*
* ��Ĭ�϶��������д������
*/
static void default_write()
{	
	fm25cl64_WRSR(dev,&fm_unprotect);
	fm25cl64_MemoryWrite(dev,0x1800,value,192);
	fm25cl64_WRSR(dev,&fm_protect);
}


/*
* ����ȡ�����ݰ�������봫������
*/
static rt_err_t data_set(float *buffer)
{
	for(int i=0;i<12;i++)
	{
		sensor_center[i].radio = *buffer++;
		sensor_center[i].offset = *buffer++;
		sensor_center[i].full = *buffer++;
		sensor_center[i].zero = *buffer++;
	}
	return RT_EOK;
}

static void fm_entry(void *parameter)
{	
	float buffer[48] = {0};
	rt_uint32_t crc_value = 0;//�鿴�洢��ȥ������
	rt_uint32_t crc_check = 0;
	
	fm25cl64_MemoryRead(dev,0x1800,buffer,192);//��F_RAM��ȡ����
	fm25cl64_MemoryRead(dev,CRC_VALUE,&crc_value,4);
	
	crc_check = crc32((rt_uint8_t *)buffer,192,0xffffffff);
	if(crc32((rt_uint8_t *)&crc_value,4,crc_check))//��F_RAM��ȡ������У��ʧ��
	{
		default_write();//д�����д���������
		crc_check = crc32((rt_uint8_t*)value,192,0xffffffff);//�õ����ݵ�CRCУ����
		crc_value = crc_check;
		fm25cl64_WRSR(dev,&fm_unprotect);//ȡ��д����
		byte_reverse((rt_uint32_t*)&crc_value);//��תcrcУ����ĸߵ�λ˳��keilС��ģʽ��
		fm25cl64_MemoryWrite(dev,CRC_VALUE,&crc_value,4);//д�����ݵ�CRCУ����
		fm25cl64_WRSR(dev,&fm_protect);//����д����
		
		fm25cl64_MemoryRead(dev,CRC_VALUE,&crc_value,4);//��ȡCRC����У��
		if(crc32((rt_uint8_t *)&crc_value,4,crc_check))//��д������У��ʧ��
		{
			rt_kprintf("FM data check error!\n");
		}
		else//��д������У��ɹ�
		{
			rt_kprintf("FM data check ok!\n");
		}
		fm25cl64_MemoryRead(dev,0x1800,buffer,192);//���´�F_RAM��ȡ����
	}

	data_set(buffer);//��ȡ�����ݷ�����������Ľṹ������
	
	while(1)
	{
		crc_check = 0;
		fm25cl64_WRSR(dev,&fm_unprotect);//ȡ��д����
		fm25cl64_MemoryWrite(dev,CRC_VALUE,&crc_check,4);//д�����ݵ�CRCУ����
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


