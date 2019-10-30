#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <fm25cl64.h>

static fm25cl64_t dev;

int fm_test()
{
  	rt_uint8_t cmd = 0xff;
	rt_uint8_t buffer[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
	rt_uint8_t test[8] = {0};
	fm25cl64_RDSR(dev,&cmd);//读取保护寄存器
	rt_kprintf("%02x\n",cmd);
	
	fm25cl64_WRSR(dev,FM25CL64_None);//取消写保护
	fm25cl64_RDSR(dev,&cmd);//读取保护寄存器
	rt_kprintf("%02x\n",cmd);
	fm25cl64_MemoryWrite(dev,0x1800,buffer,8);//写入数据的CRC校验码

	fm25cl64_WRSR(dev,FM25CL64_All);//开启写保护
	fm25cl64_RDSR(dev,&cmd);//读取保护寄存器
	rt_kprintf("%02x\n",cmd);
	fm25cl64_MemoryRead(dev,0x1800,test,8);//从F_RAM读取数据
		
	rt_uint8_t *data=test;
	for(int i=0;i<8;i++)
	{
		rt_kprintf("%x\n",*data++);
	}
	  
	return 0;
}
MSH_CMD_EXPORT(fm_test,fm write then read);

int fm_init()
{
	rt_err_t resault  = RT_ERROR;
	
	dev = rt_calloc(1,sizeof(fm25cl64_t));
	
	dev->bus_name = "spi4";
	dev->device_name = "fm25cl64";
	
	resault = fm25cl64_init(dev,GPIOE,GPIO_PIN_3);
	if(resault==RT_ERROR)
	{
		rt_kprintf("FM device init failed!\n");
		rt_free(dev);
	}
	else
	{
		rt_kprintf("FM device init success!\n");
	}
	
	return resault;
}
MSH_CMD_EXPORT(fm_init,init fm device);


