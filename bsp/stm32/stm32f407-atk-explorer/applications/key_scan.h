/*
 * Copyright (c) 2019, ShangHai ChuangLi Team
 *
 * Change Logs:
 * Date           Author        Notes
 * 2019-07-08     Li            first version
 */

#ifndef __KEY_SCAN_H__
#define __KEY_SCAN_H__

#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>

/*		设备名称	*/
#define key_device_name	"key_block"

/*		定时器名称及超时时间	*/
#define key_timer_name	"key_scan"
#define	scan_overtime	10	//

/*		按键数量 根据实际修改	*/
#define key_amount		3	//当前使用的按键数量
#define key_amount_max	32	//最多可使用的按键数量（当前最大支持32个独立按键）

/*		长按、双击时间		*/
#define LONG_TICK		200	//	T = LONG_TICK * scan_overtime
#define DOUBLE_TICK		50	//	T = DOUBLE_TICK * scan_overtime
#define DEBOUNCE_TICK	5	//	T = DEBOUNCE_TICK * scan_overtime	

/*		按键引脚号	根据实际修改	*/
static rt_uint8_t pin_list[key_amount] = {68,67,66};//按键引脚索引号


struct key_struct
{
	struct rt_device parent;
	char *name;
};

enum key_state//按键事件
{
	KEY_NO = 0,
	KEY_CLICK,
	KEY_DOUBLE,
	KEY_LONG
};

static void scan_entry(void *parameter);//定时器超时函数

rt_err_t rt_hw_key_init(void);//注册 key 功能设备

#endif	//key_scan.h
