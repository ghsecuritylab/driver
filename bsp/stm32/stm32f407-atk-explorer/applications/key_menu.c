#include "key_driver.h"
extern rt_uint8_t key_value[key_amount+1];

static void test_entry(void *para)
{	
	while(1)
	{
		rt_kprintf("%d %d %d %d\n",key_value[0],key_value[1],key_value[2],key_value[3]);
		rt_memset(key_value,0,4);
		rt_thread_delay(2000);
	}
}

int key_test(void)
{
	rt_thread_t tid;
	key_init();
	tid = rt_thread_create("test",test_entry,RT_NULL,512,15,20);
	rt_thread_startup(tid);
	return 0;
}
MSH_CMD_EXPORT(key_test,test include four state);

/* ==============使用说明=================*/
/*
* 1、头文件修改
*    按键驱动内部创建一个线程用于扫描按键状态
*    该线程名称、优先级、循环间隔参数可修改
*    
*    按键数量需要根据实际修改，最多支持32个按键
*
*    双击、长按、消抖的时间间隔可修改，实际时间是设置值与线程循环时间之积
*
*    条件定义 根据按键的实际电路设置，如果按键低电平有效接口初始状态设置上拉
*
*    pin_list数组存储按键的引脚号，具有顺序性
*
*
* 2、变量说明
*    pin_list数组与key_value数组的顺序是对应的
*    当pin_list数组内0号元素对应按键触发单击，key_value数组的0号元素数值变为1
*
*    key_value默认状态为无触发状态，数值为0;单击数值为1;双击数值为2;长按数值为3
*    key_value比pin_list多一个元素，最后一个元素用于存储组合按键结果
*    key_value为rt_uint8_t类型，而且0状态被用于默认状态，所以最多存储255个组合按键结果
*
* 3、使用说明
*    只留给外部一个接口函数key_init用于初始化引脚和创建扫面线程
*    线程创建成功返回RT_EOK，创建失败返回RT_ERROR
*    
*    按键触发结果存储于key_value数组，可供外部直接引用
*    读取key_value数据后必须把对应数据清零，否则不会记录后续该元素对应按键的触发结果
*
*    对于组合按键设置需要在switch函数里面注意添加
*    case值需要根据案件索引号数组中的位置计算;低位在前，高位在后
*    每一个组合对应的返回值保证唯一，结果由使用者自己对应
*
*/
