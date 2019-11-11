#ifndef __LED_H__
#define __LED_H__

#include <board.h>

/*************���������궨��*************/
#define PIN_HIGH_ENABLE		//�������LED��Ч��ƽ
#define led_amount	2



struct led
{
	rt_base_t 	pin_number;//LED����������
	rt_uint16_t pin_stander;//�߰�λ�洢һ����˸��ʽ���Ͱ�λ�洢һ����˸��ʽ
							//�洢��ʽ�У�����λ�洢�仯����������λ�洢��ʱʱ��
	rt_uint8_t 	pin_cont;//����λ��¼��ʱʱ�䣬����λ��¼LED״̬�仯����
};
typedef struct led *led_t;

typedef enum
{
	LED_OFF			= 0x00,		//led����
	LED_ON			= 0x01,		//led����
	LED_FAST		= 0x02,		//ָʾ����10HzƵ����˸  ������100ms����100ms
	LED_SLOW		= 0x03,		//ָʾ����2.5HzƵ����˸ ������400ms����400ms
	LED_ONE			= 0x04,		//ָʾ����200ms����1300ms
	LED_DOUBLE		= 0x05,		//ָʾ����200ms����200ms����200ms����900ms
	LED_THREE		= 0x06,		//ָʾ����200ms����200ms����200ms����200ms����200ms����500ms
	LED_VERY_SLOW	= 0x07		//ָʾ����1s����1s
}LED_TYPE;


rt_err_t led_start(rt_base_t pin_num,LED_TYPE led_flash);
void led_stop(rt_base_t pin_num);

int led_init(void);
void led_deinit(void);

#endif //led.h
