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

/* ==============ʹ��˵��=================*/
/*
* 1��ͷ�ļ��޸�
*    ���������ڲ�����һ���߳�����ɨ�谴��״̬
*    ���߳����ơ����ȼ���ѭ������������޸�
*    
*    ����������Ҫ����ʵ���޸ģ����֧��32������
*
*    ˫����������������ʱ�������޸ģ�ʵ��ʱ��������ֵ���߳�ѭ��ʱ��֮��
*
*    �������� ���ݰ�����ʵ�ʵ�·���ã���������͵�ƽ��Ч�ӿڳ�ʼ״̬��������
*
*    pin_list����洢���������źţ�����˳����
*
*
* 2������˵��
*    pin_list������key_value�����˳���Ƕ�Ӧ��
*    ��pin_list������0��Ԫ�ض�Ӧ��������������key_value�����0��Ԫ����ֵ��Ϊ1
*
*    key_valueĬ��״̬Ϊ�޴���״̬����ֵΪ0;������ֵΪ1;˫����ֵΪ2;������ֵΪ3
*    key_value��pin_list��һ��Ԫ�أ����һ��Ԫ�����ڴ洢��ϰ������
*    key_valueΪrt_uint8_t���ͣ�����0״̬������Ĭ��״̬���������洢255����ϰ������
*
* 3��ʹ��˵��
*    ֻ�����ⲿһ���ӿں���key_init���ڳ�ʼ�����źʹ���ɨ���߳�
*    �̴߳����ɹ�����RT_EOK������ʧ�ܷ���RT_ERROR
*    
*    ������������洢��key_value���飬�ɹ��ⲿֱ������
*    ��ȡkey_value���ݺ����Ѷ�Ӧ�������㣬���򲻻��¼������Ԫ�ض�Ӧ�����Ĵ������
*
*    ������ϰ���������Ҫ��switch��������ע�����
*    caseֵ��Ҫ���ݰ��������������е�λ�ü���;��λ��ǰ����λ�ں�
*    ÿһ����϶�Ӧ�ķ���ֵ��֤Ψһ�������ʹ�����Լ���Ӧ
*
*/
