#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <fm25cl64.h>
#include <drv_spi.h>


int fm25cl64_init()
{
	rt_pin_mode(GET_PIN(B,12),PIN_MODE_OUTPUT);
	rt_pin_write(GET_PIN(B,12),PIN_HIGH);
	
	rt_hw_spi_device_attach("spi2","fm25cl64",GPIOB,GPIO_PIN_12);
	
	
	return 0;
}
INIT_DEVICE_EXPORT(fm25cl64_init);
