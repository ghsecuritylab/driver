#ifndef __KEY_DRIVER_H__
#define __KEY_DRIVER_H__

#include <board.h>
#include <rtthread.h>

/*		�߳����Ƽ�ɨ��ʱ��  	*/
#define key_thread_name	"key_scan"
#define key_priority	10
#define	scan_overtime	10	//

/*		�������� ����ʵ���޸�	*/
#define key_amount		3	//��ǰʹ�õİ�������
#define key_amount_max	32	//����ʹ�õİ�����������ǰ���֧��32������������

/*		������˫��������ʱ��		*/
#define LONG_TICK		200	//	T = LONG_TICK * scan_overtime
#define DOUBLE_TICK		50	//	T = DOUBLE_TICK * scan_overtime
#define DEBOUNCE_TICK	5	//	T = DEBOUNCE_TICK * scan_overtime	

#if 1	
	#define PIN_MODE_STATE	PIN_MODE_INPUT_PULLUP //�����͵�ƽ��Ч
	#define	PIN_VALID		0
#else
	#define PIN_MODE_STATE	PIN_MODE_INPUT_PULLDOWN //�����ߵ�ƽ��Ч
	#define	PIN_VALID		1
#endif
/*		�������ź�	����ʵ���޸�	*/
static rt_uint8_t pin_list[key_amount] = {68,67,66};//��������������

enum key_state//�����¼�
{
	KEY_NO = 0,
	KEY_CLICK,
	KEY_DOUBLE,
	KEY_LONG
};

static void key_entry(void *parameter);//ɨ�谴������

rt_err_t key_init(void);

#endif	// key_driver.h

