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

/*		�豸����	*/
#define key_device_name	"key_block"

/*		��ʱ�����Ƽ���ʱʱ��	*/
#define key_timer_name	"key_scan"
#define	scan_overtime	10	//

/*		�������� ����ʵ���޸�	*/
#define key_amount		3	//��ǰʹ�õİ�������
#define key_amount_max	32	//����ʹ�õİ�����������ǰ���֧��32������������

/*		������˫��ʱ��		*/
#define LONG_TICK		200	//	T = LONG_TICK * scan_overtime
#define DOUBLE_TICK		50	//	T = DOUBLE_TICK * scan_overtime
#define DEBOUNCE_TICK	5	//	T = DEBOUNCE_TICK * scan_overtime	

/*		�������ź�	����ʵ���޸�	*/
static rt_uint8_t pin_list[key_amount] = {68,67,66};//��������������


struct key_struct
{
	struct rt_device parent;
	char *name;
};

enum key_state//�����¼�
{
	KEY_NO = 0,
	KEY_CLICK,
	KEY_DOUBLE,
	KEY_LONG
};

static void scan_entry(void *parameter);//��ʱ����ʱ����

rt_err_t rt_hw_key_init(void);//ע�� key �����豸

#endif	//key_scan.h
