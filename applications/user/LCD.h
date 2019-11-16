#ifndef __LCD_H__
#define __LCD_H__
#include <board.h>

#define LCDDIR	1	// 0:竖屏 	1:横屏
#define DEFAULT_BACK	0xffff

/* =========== Color ========== */
#define RED     	0xf800
#define GREEN   	0x07e0
#define BLUE    	0x001f
#define YELLOW  	0xffe0
#define CYAN    	0x07ff
#define MAGENTA 	0xf81f
#define BLACK   	0x0000
#define WHITE   	0xffff
#define GRAY    	0x8410

#define hi_re		0xf800
#define hi_or 		0xfc00
#define hi_ye 		0xffb0
#define hi_yg 		0x87e0
#define hi_gr 		0x07e0
#define hi_gc 		0x07f0
#define hi_cy 		0x07ff
#define hi_cb 		0x041f
#define hi_bl 		0x001f
#define hi_bm 		0x801f
#define hi_ma 		0xf8f1
#define hi_mr 		0xf810

#define mid_re 		0x8000
#define mid_or 		0x8200
#define mid_ye 		0x8400
#define mid_yg 		0x4400
#define mid_gr 		0x0400
#define mid_gc 		0x0408
#define mid_cy 		0x0410
#define mid_cb 		0x0210
#define mid_bl 		0x0010
#define mid_bm 		0x4010
#define mid_ma 		0x8010
#define mid_mr 		0x8008

#define GRAY00  	0x0000
#define GRAY01  	0x1082
#define GRAY02  	0x2104
#define GRAY03  	0x3186
#define GRAY04  	0x4208
#define GRAY05  	0x528a
#define GRAY06  	0x630c
#define GRAY07  	0x738e
#define GRAY08  	0x8410
#define GRAY09  	0x9492
#define GRAY10  	0xa514
#define GRAY11  	0xb596
#define GRAY12  	0xc618
#define GRAY13  	0xd69a
#define GRAY14  	0xe71c
#define GRAY15  	0xffff


/* ========= Cmd List ========= */
#define NOP		(0x00)	// 空指令
#define RESET	(0x01)	// 复位指令
#define SLPIN	(0x10)	// 进入睡眠
#define SLPOUT	(0x11)	// 退出睡眠
#define CASET	(0x2A)	// 列地址设置
#define RASET	(0x2B)	// 行地址设置
#define RAMWR	(0x2C)	// 写数据
#define COLMOD	(0x3A)	// 色彩格式
#define MRACTL	(0x36)	// RAM地址控制


struct lcd
{
	struct rt_spi_device *spi_dev;
	char *name;
	rt_uint8_t bla_pin;
	rt_uint8_t dc_pin;
	rt_uint8_t rst_pin;
};
typedef struct lcd *lcd_t;

void lcd_ColorSet(rt_uint16_t paint);
void lcd_DrawPoint(lcd_t dev,rt_uint16_t x,rt_uint16_t y,rt_uint16_t pen);
void lcd_FillRect(lcd_t dev,rt_uint16_t x0,rt_uint16_t y0,rt_uint16_t x1,rt_uint16_t y1,rt_uint16_t color);

void lcd_ShowChar(lcd_t dev,rt_uint16_t x,rt_uint16_t y,char data);
void lcd_ShowStr(lcd_t dev,rt_uint16_t x,rt_uint16_t y,char *data);

rt_err_t lcd_init(lcd_t dev,const char *spi_bus,GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin);
void lcd_Deinit(lcd_t dev);

#endif //lcd.h
