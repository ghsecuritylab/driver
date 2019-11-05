#include <rtthread.h>
#include <rtdevice.h>
#include "lcd.h"
static lcd_t dev;


void lcd_example()
{
	dev = rt_calloc(1,sizeof(struct lcd));
	if(dev == RT_NULL)
	{
		rt_kprintf("lcd memory failed!\n");
		return;
	}
	dev->bla_pin = GET_PIN(D, 13);
	dev->dc_pin = GET_PIN(D,12);
	dev->name = "lcd";
	dev->rst_pin = GET_PIN(A,9);
	
	if(lcd_init(dev,"spi1",GPIOA,GPIO_PIN_10) == RT_ERROR)
	{
		lcd_Deinit(dev);
		rt_kprintf("lcd_init error!\n");
		return;
	}
	else
		rt_kprintf("lcd_init ok!\n");
	lcd_write(dev);
	lcd_Deinit(dev);
}
MSH_CMD_EXPORT(lcd_example,lcd test);
