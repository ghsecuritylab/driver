#ifndef __LCD_H__
#define __LCD_H__
#include <board.h>

/* =========== Color ========== */
#define RED     0xf800
#define GREEN   0x07e0
#define BLUE    0x001f
#define YELLOW  0xffe0
#define CYAN    0x07ff
#define MAGENTA 0xf81f
#define BLACK   0x0000
#define WHITE   0xffff
#define GRAY    0x8410

/* ========= Cmd List ========= */
#define NOP		(0x00)	// ��ָ��
#define RESET	(0x01)	// ��λָ��
#define SLPIN	(0x10)	// ����˯��
#define SLPOUT	(0x11)	// �˳�˯��
#define CASET	(0x2A)	// �е�ַ����
#define RASET	(0x2B)	// �е�ַ����
#define RAMWR	(0x2C)	// д����
#define COLMOD	(0x3A)	// ɫ�ʸ�ʽ


struct lcd
{
	struct rt_spi_device *spi_dev;
	char *name;
	rt_uint8_t bla_pin;
	rt_uint8_t dc_pin;
	rt_uint8_t rst_pin;
};
typedef struct lcd *lcd_t;



rt_err_t lcd_write(lcd_t dev);
rt_err_t lcd_init(lcd_t dev,const char *spi_bus,GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin);
void lcd_Deinit(lcd_t dev);

#endif //lcd.h
