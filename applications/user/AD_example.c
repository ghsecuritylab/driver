#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <ad7739.h>

static AD7739_t dev;

void AD_test()
{
	rt_uint8_t cmd;
	rt_uint8_t buffer[24];

	ad7739_read(dev,AD7739_REV,&cmd,1);
	rt_kprintf("%02X\n",cmd);
	
	ad7739_channel_read(dev,buffer,3);
	rt_kprintf("%s\n",buffer);
	
}
MSH_CMD_EXPORT(AD_test,test ad device read);
	
void AD_init(void)
{
	dev = rt_calloc(1,sizeof(struct AD7739));
	
	dev->adc_device_name = "AD7739";
	dev->channel_enable	 = 0x03;
	dev->ready_pin		 = GET_PIN(B,7);
	dev->reset_pin		 = GET_PIN(B,6);
	
	ad7739_init("spi1",dev,GPIOB,GPIO_PIN_5);
	
}
MSH_CMD_EXPORT(AD_init, ad7739 device init);

