#ifndef __KEY_DRIVER_H__
#define __KEY_DRIVER_H__

#include <board.h>
#include <rtthread.h>

/*		线程名称及扫描时间  	*/
#define key_thread_name	"key_scan"
#define key_priority	10
#define	scan_overtime	10	//

/*		按键数量 根据实际修改	*/
#define key_amount		3	//当前使用的按键数量
#define key_amount_max	32	//最多可使用的按键数量（当前最大支持32个独立按键）

/*		长按、双击、消抖时间		*/
#define LONG_TICK		200	//	T = LONG_TICK * scan_overtime
#define DOUBLE_TICK		50	//	T = DOUBLE_TICK * scan_overtime
#define DEBOUNCE_TICK	5	//	T = DEBOUNCE_TICK * scan_overtime	

#if 1	
	#define PIN_MODE_STATE	PIN_MODE_INPUT_PULLUP //按键低电平有效
	#define	PIN_VALID		0
#else
	#define PIN_MODE_STATE	PIN_MODE_INPUT_PULLDOWN //按键高电平有效
	#define	PIN_VALID		1
#endif
/*		按键引脚号	根据实际修改	*/
static rt_uint8_t pin_list[key_amount] = {68,67,66};//按键引脚索引号

enum key_state//按键事件
{
	KEY_NO = 0,
	KEY_CLICK,
	KEY_DOUBLE,
	KEY_LONG
};

static void key_entry(void *parameter);//扫描按键函数

rt_err_t key_init(void);

#endif	// key_driver.h

