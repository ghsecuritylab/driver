#ifndef __LED_H__
#define __LED_H__

#include <board.h>

/*************功能启动宏定义*************/
#define PIN_HIGH_ENABLE		//定义点亮LED有效电平
#define led_amount	2



struct led
{
	rt_base_t 	pin_number;//LED引脚索引号
	rt_uint16_t pin_stander;//高八位存储一种闪烁方式，低八位存储一种闪烁方式
							//存储方式中：高四位存储变化次数，低四位存储定时时间
	rt_uint8_t 	pin_cont;//低四位记录定时时间，高四位记录LED状态变化次数
};
typedef struct led *led_t;

typedef enum
{
	LED_OFF			= 0x00,		//led常灭
	LED_ON			= 0x01,		//led常亮
	LED_FAST		= 0x02,		//指示灯以10Hz频率闪烁  ，即亮100ms，灭100ms
	LED_SLOW		= 0x03,		//指示灯以2.5Hz频率闪烁 ，即亮400ms，灭400ms
	LED_ONE			= 0x04,		//指示灯亮200ms，灭1300ms
	LED_DOUBLE		= 0x05,		//指示灯亮200ms，灭200ms，亮200ms，灭900ms
	LED_THREE		= 0x06,		//指示灯亮200ms，灭200ms，亮200ms，灭200ms，亮200ms，灭500ms
	LED_VERY_SLOW	= 0x07		//指示灯亮1s，灭1s
}LED_TYPE;


rt_err_t led_start(rt_base_t pin_num,LED_TYPE led_flash);
void led_stop(rt_base_t pin_num);

int led_init(void);
void led_deinit(void);

#endif //led.h
