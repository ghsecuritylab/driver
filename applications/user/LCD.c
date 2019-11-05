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
static rt_err_t lcd_DataIn(lcd_t dev,rt_uint8_t data)
{
	rt_pin_write(dev->dc_pin,PIN_HIGH);//设置为写数据
	
	if(rt_spi_send(dev->spi_dev,&data,1))
		return RT_EOK;
	else
		return RT_ERROR;
}

/*
* 函数名称：lcd_DrawPoint
* 函数功能：绘制点
* 参数描述：x ,y 点的位置；color 点的颜色
*/
void lcd_DrawPoint(lcd_t dev,rt_uint16_t x,rt_uint16_t y,rt_uint16_t color)
{
	lcd_CtrlIn(dev,RASET);// 设置行地址
	lcd_DataIn(dev,x>>8);
	lcd_DataIn(dev,x);
	lcd_DataIn(dev,x>>8);
	lcd_DataIn(dev,x);
	lcd_CtrlIn(dev,CASET);// 设置列地址
	lcd_DataIn(dev,y>>8);
	lcd_DataIn(dev,y);
	lcd_DataIn(dev,y>>8);
	lcd_DataIn(dev,y);
	
	lcd_CtrlIn(dev,RAMWR);
	lcd_DataIn(dev,color>>8);
	lcd_DataIn(dev,color);
}

/*
* 
* 
*/
void lcd_DrawLine(lcd_t dev,rt_uint16_t x0,rt_uint16_t y0,rt_uint16_t x1,rt_uint16_t y1,rt_uint16_t color)
{
	
}

/*
* 函数名称：lcd_FillRect
* 函数功能：以某种颜色填充一块矩形
* 参数说明：x 行起始地址；y 列起始地址；length 长度；width 宽度；color 填充色
*/
void lcd_FillRect(lcd_t dev,rt_uint16_t x,rt_uint16_t y,rt_uint16_t length,rt_uint16_t width,rt_uint16_t color)
{
	lcd_CtrlIn(dev,RASET);// 设置行地址
	lcd_DataIn(dev,x>>8);
	lcd_DataIn(dev,x);
	lcd_DataIn(dev,length>>8);
	lcd_DataIn(dev,length);
	lcd_CtrlIn(dev,CASET);// 设置列地址
	lcd_DataIn(dev,y>>8);
	lcd_DataIn(dev,y);
	lcd_DataIn(dev,width>>8);
	lcd_DataIn(dev,width);
	
	lcd_CtrlIn(dev,RAMWR);
	for(int i=0;i<width;i++)
	{
		for(int j=0;j<length;j++)
		{
			lcd_DataIn(dev,color>>8);
			lcd_DataIn(dev,color);
		}
	}
}

rt_err_t lcd_write(lcd_t dev,rt_uint8_t value)
{

	lcd_CtrlIn(dev,0x51);
	lcd_DataIn(dev,value);
	
	return RT_EOK;
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
	
	lcd_CtrlIn(dev,SLPOUT);//退出睡眠
	rt_thread_delay(200);
	lcd_CtrlIn(dev,MRACTL);
	lcd_DataIn(dev,0x60);
	lcd_CtrlIn(dev,COLMOD);
	lcd_DataIn(dev,0x55);
	lcd_CtrlIn(dev,0x29);
	lcd_FillRect(dev,0,0,239,319,0xffff);
	
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
