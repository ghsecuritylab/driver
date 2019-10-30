/*
 * Copyright (c) 2019, ShangHai ChuangLi Team
 *
 * Change Logs:
 * Date           Author        Notes
 * 2019-07-08     Li            first version
 */

#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <key_scan.h>

static 	rt_timer_t scan = RT_NULL;//定时器句柄

static rt_uint8_t key_value[key_amount+1]={0};//按键功能存储数组

static rt_uint32_t pin_status = 0;//当前按键引脚状态，32位标识32个按键
static rt_err_t state=0;//状态机状态
struct key_struct key_creat =
{
	.parent = 0,
	.name = key_device_name
};//key 设备

/*
* 所有按键的引脚初始化
* 输入参数 *pin ：存有按键号的数组首地址
*/
static void stm32_key_gpio_init(rt_uint8_t *pin)
{
	rt_uint8_t *p = RT_NULL;//定义类型指针
	p = pin;//取索引号数组首地址
	for(int i=0;i<key_amount;i++)
	{
		rt_pin_mode(*p,PIN_MODE_INPUT_PULLUP);
		p++;//指向下一个索引号
	}
}

/*
* 函数名称：stm32_key_init
* 函数功能：创建 key 功能的定时器
* 返回参数：创建成功 --> RT_EOK
			创建失败 --> RT_ERROR
*/
static rt_err_t stm32_key_init(struct rt_device *dev)
{	
	RT_ASSERT(dev!=RT_NULL);
	
	scan = rt_timer_create(key_timer_name,scan_entry,RT_NULL,scan_overtime,
							RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
	if(scan!=RT_NULL)
	{
		return RT_EOK;
	}
	else
	{
		return RT_ERROR;
	}
}

/*
* 函数名称：stm32_key_start
* 输入参数：oflag	1-->开启; 0-->无操作
* 函数功能：启动扫描定时器
*/
static rt_err_t stm32_key_open(struct rt_device *dev, rt_uint16_t oflag)
{
	RT_ASSERT(dev!=RT_NULL);
	if(oflag)
	{
		rt_timer_start(scan);
		return RT_EOK;
	}
	else
	{
		return RT_ERROR;
	}	
}

/*
* 函数名称：stm32_key_stop
* 函数功能：停止扫描定时器
*/
static rt_err_t stm32_key_close(struct rt_device *dev)
{
	RT_ASSERT(dev!=RT_NULL);
	
	rt_timer_stop(scan);
	
	return RT_EOK;
}

/*
* 函数名称：stm32_key_read
* 输入参数：pos		起读位号
			*buffer 获得所有按键状态的缓冲数组
			size 	总读取位数
* 函数功能：将32个按键状态读取到指定数组
*/
static rt_size_t stm32_key_read(struct rt_device *dev, rt_off_t pos, void *buffer, rt_size_t size)
{
	rt_uint8_t *p=buffer;
	
	RT_ASSERT(dev!=RT_NULL);
	
	for(int i=pos;i<size;i++)
	{
		*p = key_value[i];
		key_value[i]=0;
		p++;
	}
	
	return RT_EOK;
}


/*
* 函数名称：rt_hw_key_init
* 函数功能：注册 key 功能设备
* 函数说明：由外部操作
*/
rt_err_t rt_hw_key_init(void)
{	
	stm32_key_gpio_init(pin_list);
	
	key_creat.parent.type = RT_Device_Class_Miscellaneous;
	key_creat.parent.rx_indicate = RT_NULL;
	key_creat.parent.tx_complete = RT_NULL;
	
	/*		操作函数		*/
	key_creat.parent.init = stm32_key_init;
	key_creat.parent.open = stm32_key_open;
	key_creat.parent.close = stm32_key_close;
	key_creat.parent.read = stm32_key_read;
	key_creat.parent.control = RT_NULL;
	key_creat.parent.write = RT_NULL;
	
	/*		私有数据		*/
	key_creat.parent.user_data = RT_NULL;
		
	return rt_device_register(&key_creat.parent,key_creat.name,RT_DEVICE_FLAG_RDONLY);
}


	


/***********************************以下为内部函数********************************************/


/*
* 函数名称：find_keyevent
* 形    参：num-->按键索引号
* 函数功能：判别 pin_list[num] 引脚的触发事件
* 返 回 值：key_no-->无事件	 ;	key_click-->单击
			key_double-->双击;	key_long--> 长按
* 说    明：处理流程根据状态机思想设计，本次设计
			只用于识别单击、双击、长按三种功能
*/
static rt_err_t find_keyevent(rt_err_t num)
{
	static rt_err_t cont=0, press_time=0;
	rt_err_t key_return = KEY_NO;
	
	switch(state)
	{
		case 0://判断按键是否被按下
			if(!rt_pin_read(pin_list[num]))//被按下
			{
				state = 1;
				press_time = 0;
			}
		break;
			
		case 1://被按下，检测松开还是长按
			if(rt_pin_read(pin_list[num]))//按键被释放
			{
				if(++cont>DEBOUNCE_TICK)
				{
					cont = 0;
					press_time = 0;
					state = 3;//进入双击检测
				}				
			}
			else//持续按下
			{
				if(++press_time>LONG_TICK)//按下时间大于长按
				{
					state = 2;
					cont = 0;
				}
			}	
		break;
		
		case 2://按下时间超过长按标准，等待释放
			if(rt_pin_read(pin_list[num]))
			{
				key_return = KEY_LONG;//返回长按参数
				if(!key_value[num])
					key_value[num]=3;
				state = 0;
			}
		break;
		
		case 3://双击识别
			if(!rt_pin_read(pin_list[num]))
			{
				if(++press_time==DEBOUNCE_TICK)
				{
					state = 0;
					key_return = KEY_DOUBLE;
					if(!key_value[num])
						key_value[num]=2;
				}
			}
			else if(++cont>DOUBLE_TICK)
			{
				press_time = 0;
				key_return = KEY_CLICK;//返回单击
				state = 0;
				if(!key_value[num])
					key_value[num]=1;
			}
		break;
			
		default://默认值
			key_return = KEY_NO;//无按键
		break;
	}
	return key_return;
}

/*
* 函数名称：key_check
* 实    参：*old_state-->存有按键状态的变量地址
* 函数功能：检测按键前后状态是否一致
* 返 回 值：1-->前后状态一致
			0-->前后状态不同
*/
static rt_err_t key_check(rt_uint32_t *old_state)
{
	rt_uint32_t pin_stable=0;
	for(int i=0;i<key_amount;i++)//轮询检测按键状态
	{
		if(rt_pin_read(pin_list[i])==0)//如果对应按键被按下
		{
			pin_stable |= 1<<i;//相应位置1
		}
	}
	if(pin_stable==*old_state)
		return RT_TRUE;//前后状态稳定，返回1
	else
	{
		*old_state=pin_stable;//前后状态不同，更新状态值
		return RT_FALSE;//返回0
	}
}

/*
* 函数名称：get_eventkey
* 形    参：state-->存有按键状态的32位变量
* 函数功能：解析状态参数中的置1位
* 返 回 值：从右往左第一个置1的位号
* 说    明： eg. 形参	 返回值
				0x0001-->return 0
				0x0100-->return 2
*/
static rt_err_t get_eventkey(rt_uint32_t state)
{
	for(int i=0;i<key_amount;i++)
	{
		if(state&(1<<i))
			return i;//返回按键在数组中的序号
	}
	return RT_FALSE;
}

/*
* 函数名称：print_key
* 实    参：*state-->存有按键状态的变量地址
* 函数功能：消抖确认全部按键状态稳定
* 返 回 值：RT_TRUE -->消抖后状态稳定，并且有按键触发
			RT_FALSE-->状态不稳定，或者消抖不成功，或者没有按键触发
*/
static rt_err_t print_key(rt_uint32_t *state)
{
	static rt_err_t num=0;
	if(key_check(state))
		num++;
	else num=0;
	if((num==DEBOUNCE_TICK)&&(*state!=0))
	{
		return RT_TRUE;
	}		
	return RT_FALSE;
}

/*
* 扫描定时器超时函数
*/
static void scan_entry(void *parameter)
{
	static int mode = 0;//扫描模式：0-->全键扫描	1-->单键扫描
	static rt_err_t press_num = 0;//记录事件按键的索引号
	
	if(pin_status==0)
		mode=0;
	
	if(mode==0)//模式0：扫描所有按键
	{
		if(print_key(&pin_status))
		{
			mode=1;
			press_num = get_eventkey(pin_status);//得到事件按键
		}
	}
	else if(mode==1)
	{
		if(find_keyevent(press_num))//按键结果被识别
		{
			mode =0;
		}	
	}
}
