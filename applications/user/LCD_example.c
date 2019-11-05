#include <rtthread.h>
#include <rtdevice.h>
#include "lcd.h"
static lcd_t dev;


void lcd_test()
{
	lcd_FillRect(dev,50,50,100,100,GREEN);
}
MSH_CMD_EXPORT(lcd_test,lcd write test);

int lcd_example()
{
	dev = rt_calloc(1,sizeof(struct lcd));
	if(dev == RT_NULL)
	{
		rt_kprintf("lcd memory failed!\n");
		return 0;
	}
	dev->bla_pin = GET_PIN(D, 13);
	dev->dc_pin = GET_PIN(D,12);
	dev->name = "lcd";
	dev->rst_pin = GET_PIN(A,9);
	
	if(lcd_init(dev,"spi1",GPIOA,GPIO_PIN_10) == RT_ERROR)
	{
		lcd_Deinit(dev);
		rt_kprintf("lcd_init error!\n");
		return 0;
	}
	else
		rt_kprintf("lcd_init ok!\n");
	return 1;
}
INIT_APP_EXPORT(lcd_example);
