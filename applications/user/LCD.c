#include <rtthread.h>
#include <rtdevice.h>
#include <math.h>
#include <drv_spi.h>

#ifdef BSP_USING_Board_LCD
#include "lcd.h"
#include "lcd_font.h"

#define LOG_TAG	"LCD2.44"
#include <drv_log.h>

static rt_uint16_t color = RED;	//Ĭ��������ɫΪ��ɫ

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
* ����LCD ����Ĭ����ɫ
*/
void lcd_ColorSet(rt_uint16_t paint)
{
	color = paint;
}

/*
* �������ƣ�lcd_DrawPoint
* �������ܣ����Ƶ�
* ����������x ,y ���λ�ã�color �����ɫ
*/
void lcd_DrawPoint(lcd_t dev,rt_uint16_t x,rt_uint16_t y,rt_uint16_t pen)
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
	lcd_DataIn(dev,pen>>8);
	lcd_DataIn(dev,pen);
}

/*
* �������ƣ�lcd_ShowChar
* �������ܣ���ָ��λ����ʾһ���ַ�
*/
void lcd_ShowChar(lcd_t dev,rt_uint16_t x,rt_uint16_t y,char data)
{	
	int index = data-' ';
	rt_uint8_t point = 0x00;
	for(int row=0;row<16;row++)
	{
		point = font16[index][row];
		for(int col=0;col<8;col++)
		{
			if((point>>col) & 0x01)
				lcd_DrawPoint(dev,x,y,color);
			else
				lcd_DrawPoint(dev,x,y,WHITE);
			y++;
		}
		y -= 8;
		x++;
	}
}

/*
* �������ƣ�lcd_ShowStr
* �������ܣ���ָ��λ����ʾ�ַ���
*/
void lcd_ShowStr(lcd_t dev,rt_uint16_t x,rt_uint16_t y,char *data)
{
	int j = rt_strlen(data);
	for(int i=0;i<j;i++)
	{
		lcd_ShowChar(dev,x,y,*data);
		data++;
		y += 8;
	}
}


/*
* �������ƣ�lcd_FillRect
* �������ܣ���ĳ����ɫ���һ�����
* ����˵����x0 ����ʼ��ַ��y0 ����ʼ��ַ��x1 �н�����ַ��y1 �н�����ַ��color ���ɫ
*/
void lcd_FillRect(lcd_t dev,rt_uint16_t x0,rt_uint16_t y0,rt_uint16_t x1,rt_uint16_t y1,rt_uint16_t color)
{
	rt_uint16_t tr = 0;
	if(x0>x1){
		tr = (rt_uint16_t)fmin(x0,x1);
		x1 = (rt_uint16_t)fmax(x0,x1);
		x0 = tr;
	}
	if(y0>y1){
		tr = (rt_uint16_t)fmin(y0,y1);
		y1 = (rt_uint16_t)fmax(y0,y1);
		y0 = tr;
	}
	
	lcd_CtrlIn(dev,RASET);// �����е�ַ
	lcd_DataIn(dev,x0>>8);
	lcd_DataIn(dev,x0);
	lcd_DataIn(dev,x1>>8);
	lcd_DataIn(dev,x1);
	lcd_CtrlIn(dev,CASET);// �����е�ַ
	lcd_DataIn(dev,y0>>8);
	lcd_DataIn(dev,y0);
	lcd_DataIn(dev,y1>>8);
	lcd_DataIn(dev,y1);
	
	lcd_CtrlIn(dev,RAMWR);// дRAM����
	for(int i=0;i<(y1-y0+1);i++)
	{
		for(int j=0;j<(x1-x0+1);j++)
		{
			lcd_DataIn(dev,color>>8);
			lcd_DataIn(dev,color);
		}
	}
}


/*
* �������ƣ�lcd_init
* �������ܣ�����ָ����Ƭѡ���ź�SPI���߳�ʼ���豸
*/
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
	rt_thread_delay(10);// reset wait for 1s
	rt_pin_write(dev->rst_pin,PIN_HIGH);
	rt_thread_delay(800);// reset wait for 800ms
	
	/*			turn on lcd	device		*/
	rt_pin_write(dev->bla_pin,PIN_HIGH);
	
	lcd_CtrlIn(dev,SLPOUT);// �˳�˯��
	rt_thread_delay(150);
	lcd_CtrlIn(dev,COLMOD);// ɫ�ʸ�ʽ
	lcd_DataIn(dev,0x55);
	lcd_CtrlIn(dev,MRACTL);// RAM ��ʽ
#if LCDDIR==0
	lcd_DataIn(dev,0x00);
	lcd_FillRect(dev,0,0,320,240,DEFAULT_BACK);// ����
#else
	lcd_DataIn(dev,0x60);
	lcd_FillRect(dev,0,0,240,320,DEFAULT_BACK);// ����
#endif	
	
	lcd_CtrlIn(dev,0x29);  // ����ʾ
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

#endif
