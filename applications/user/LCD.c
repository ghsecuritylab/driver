#include <rtthread.h>
#include <rtdevice.h>
#include <drv_spi.h>
#include "lcd.h"

#define LOG_TAG	"LCD2.44"
#include <drv_log.h>

/*
* 函数名称：lcd_CtrlIn
* 函数说明：向LCD写入控制指令
*/
static rt_err_t lcd_CtrlIn(lcd_t dev,rt_uint8_t cmd)
{
	rt_pin_write(dev->dc_pin,PIN_LOW);//设置为写控制指令
	if(rt_spi_send(dev->spi_dev,&cmd,1))
		return RT_EOK;
	else
		return RT_ERROR;
}

/*
* 函数名称：lcd_DataIn
* 函数说明：向LCD写入数据
*/
static rt_err_t lcd_DataIn(lcd_t dev,void *data,rt_size_t size)
{
	rt_pin_write(dev->dc_pin,PIN_HIGH);//设置为写数据
	rt_spi_send(dev->spi_dev,data,size);
}

rt_err_t lcd_write(lcd_t dev)
{
	rt_uint8_t data[9] = {0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x0f,55};
	lcd_CtrlIn(dev,CASET);
	lcd_DataIn(dev,data,4);
	lcd_CtrlIn(dev,RASET);
	lcd_DataIn(dev,&data[4],4);
	lcd_CtrlIn(dev,RAMWR);
	lcd_DataIn(dev,&data[8],1);
}

rt_err_t lcd_init(lcd_t dev,const char *spi_bus,GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
{
	/*			init lcd pin			*/
	rt_pin_mode(dev->rst_pin,PIN_MODE_OUTPUT);
	rt_pin_mode(dev->bla_pin,PIN_MODE_OUTPUT);
	rt_pin_mode(dev->dc_pin,PIN_MODE_OUTPUT);
	
	rt_hw_spi_device_attach(spi_bus,dev->name,GPIOx,GPIO_Pin);// bind spi device
	
	dev->spi_dev = (struct rt_spi_device*)rt_device_find(dev->name);
	if(dev->spi_dev == RT_NULL)
	{
		lcd_Deinit(dev);
		LOG_E("lcd device find falied!\n");
		return RT_ERROR;
	}
	
	/*			spi bus config			*/
	{
		struct rt_spi_configuration cfg;
		cfg.data_width = 8;
		cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
		cfg.max_hz = 6 * 1000 * 1000;
		rt_spi_configure(dev->spi_dev,&cfg);
	}
	
	/*			reset lcd device		*/
	rt_pin_write(dev->rst_pin,PIN_LOW);
	rt_thread_delay(1000);// reset wait for 1s
	rt_pin_write(dev->rst_pin,PIN_HIGH);
	
	/*			turn on lcd	device		*/
	rt_pin_write(dev->bla_pin,PIN_HIGH);
	
	return RT_EOK;
}

/*
* 函数名称：lcd_Deinit
* 函数功能：释放lcd设备句柄
*/
void lcd_Deinit(lcd_t dev)
{
	RT_ASSERT(dev!=RT_NULL);
	rt_free(dev);
}
