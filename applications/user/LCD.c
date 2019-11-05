#include <rtthread.h>
#include <rtdevice.h>
#include <drv_spi.h>
#include "lcd.h"

#define LOG_TAG	"LCD2.44"
#include <drv_log.h>

/*
* �������ƣ�lcd_CtrlIn
* ����˵������LCDд�����ָ��
*/
static rt_err_t lcd_CtrlIn(lcd_t dev,rt_uint8_t cmd)
{
	rt_pin_write(dev->dc_pin,PIN_LOW);//����Ϊд����ָ��

	if(rt_spi_send(dev->spi_dev,&cmd,1))
		return RT_EOK;
	else
		return RT_ERROR;
}

/*
* �������ƣ�lcd_DataIn
* ����˵������LCDд������
*/
static rt_err_t lcd_DataIn(lcd_t dev,rt_uint8_t data)
{
	rt_pin_write(dev->dc_pin,PIN_HIGH);//����Ϊд����
	
	if(rt_spi_send(dev->spi_dev,&data,1))
		return RT_EOK;
	else
		return RT_ERROR;
}

/*
* �������ƣ�lcd_DrawPoint
* �������ܣ����Ƶ�
* ����������x ,y ���λ�ã�color �����ɫ
*/
void lcd_DrawPoint(lcd_t dev,rt_uint16_t x,rt_uint16_t y,rt_uint16_t color)
{
	lcd_CtrlIn(dev,RASET);// �����е�ַ
	lcd_DataIn(dev,x>>8);
	lcd_DataIn(dev,x);
	lcd_DataIn(dev,x>>8);
	lcd_DataIn(dev,x);
	lcd_CtrlIn(dev,CASET);// �����е�ַ
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
* �������ƣ�lcd_FillRect
* �������ܣ���ĳ����ɫ���һ�����
* ����˵����x ����ʼ��ַ��y ����ʼ��ַ��length ���ȣ�width ��ȣ�color ���ɫ
*/
void lcd_FillRect(lcd_t dev,rt_uint16_t x,rt_uint16_t y,rt_uint16_t length,rt_uint16_t width,rt_uint16_t color)
{
	lcd_CtrlIn(dev,RASET);// �����е�ַ
	lcd_DataIn(dev,x>>8);
	lcd_DataIn(dev,x);
	lcd_DataIn(dev,length>>8);
	lcd_DataIn(dev,length);
	lcd_CtrlIn(dev,CASET);// �����е�ַ
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
	
	lcd_CtrlIn(dev,SLPOUT);//�˳�˯��
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
* �������ƣ�lcd_Deinit
* �������ܣ��ͷ�lcd�豸���
*/
void lcd_Deinit(lcd_t dev)
{
	RT_ASSERT(dev!=RT_NULL);
	rt_free(dev);
}
