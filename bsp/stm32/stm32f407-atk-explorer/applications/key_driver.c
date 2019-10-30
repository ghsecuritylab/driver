#include "key_driver.h"

rt_uint8_t key_value[key_amount+1]={0};//�������ܴ洢����

static rt_uint32_t pin_status = 0;//��ǰ��������״̬��32λ��ʶ32������
static rt_err_t state=0;//״̬��״̬
static rt_uint8_t flag =0;//��¼�����¼��������Ƕ��
/*
* �������ƣ�key_gpio_init
* �������ܣ���ʼ�����õ����а�����GPIO
*/
static void key_gpio_init(rt_uint8_t *pin)
{
	rt_uint8_t *p = RT_NULL;//��������ָ��
	p = pin;//ȡ�����������׵�ַ
	for(int i=0;i<key_amount;i++)
	{
		rt_pin_mode(*p,PIN_MODE_STATE);
		p++;//ָ����һ��������
	}
}

rt_err_t key_init(void)
{
	rt_thread_t tid;
	key_gpio_init(pin_list);//��ʼ���ӿ�
	tid = rt_thread_create(key_thread_name,key_entry,RT_NULL,1024,key_priority,20);
	if(tid!=RT_NULL)
	{
		rt_kprintf("KEY Driver's Timer init success!\n");
		rt_thread_startup(tid);
		return RT_EOK;
	}
	else
	{
		rt_kprintf("KEY Driver's Timer init failed!\n");
		return RT_ERROR;
	}
}

/***********************************����Ϊ�ڲ�����********************************************/


/*
* �������ƣ�find_keyevent
* ��    �Σ�num-->����������
* �������ܣ��б� pin_list[num] ���ŵĴ����¼�
* �� �� ֵ��key_no-->���¼�	 ;	key_click-->����
			key_double-->˫��;	key_long--> ����
* ˵    �����������̸���״̬��˼����ƣ��������
			ֻ����ʶ�𵥻���˫�����������ֹ���
*/
static rt_err_t find_keyevent(rt_err_t num)
{
	static rt_err_t cont=0, press_time=0;
	rt_err_t key_return = KEY_NO;
	
	switch(state)
	{
		case 0://�жϰ����Ƿ񱻰���
			if(rt_pin_read(pin_list[num])==PIN_VALID)//������
			{
				state = 1;
				press_time = 0;
			}
		break;
			
		case 1://�����£�����ɿ����ǳ���
			if(rt_pin_read(pin_list[num])!=PIN_VALID)//�������ͷ�
			{
				if(++cont>DEBOUNCE_TICK)
				{
					cont = 0;
					press_time = 0;
					state = 3;//����˫�����
				}				
			}
			else//��������
			{
				if(++press_time>LONG_TICK)//����ʱ����ڳ���
				{
					state = 2;
					cont = 0;
				}
			}	
		break;
		
		case 2://����ʱ�䳬��������׼���ȴ��ͷ�
			if(rt_pin_read(pin_list[num])!=PIN_VALID)
			{
				key_return = KEY_LONG;//���س�������
				if(!key_value[num])
					key_value[num]=3;
				state = 0;
			}
		break;
		
		case 3://˫��ʶ��
			if(rt_pin_read(pin_list[num])==PIN_VALID)
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
				key_return = KEY_CLICK;//���ص���
				state = 0;
				if(!key_value[num])
					key_value[num]=1;
			}
		break;
			
		default://Ĭ��ֵ
			key_return = KEY_NO;//�ް���
		break;
	}
	return key_return;
}

/*
* �������ƣ�key_check
* ʵ    �Σ�*old_state-->���а���״̬�ı�����ַ
* �������ܣ���ⰴ��ǰ��״̬�Ƿ�һ��
* �� �� ֵ��1-->ǰ��״̬һ��
			0-->ǰ��״̬��ͬ
*/
static rt_err_t key_check(rt_uint32_t *old_state)
{
	rt_uint32_t pin_stable=0;
	for(int i=0;i<key_amount;i++)//��ѯ��ⰴ��״̬
	{
		if(rt_pin_read(pin_list[i])==PIN_VALID)//�����Ӧ����������
		{
			pin_stable |= 1<<i;//��Ӧλ��1
		}
	}
	if(pin_stable==*old_state)
		return RT_TRUE;//ǰ��״̬�ȶ�������1
	else
	{
		*old_state=pin_stable;//ǰ��״̬��ͬ������״ֵ̬
		return RT_FALSE;//����0
	}
}

/*
* �������ƣ�get_eventkey
* ��    �Σ�state-->���а���״̬��32λ����
* �������ܣ�����״̬�����е���1λ
* �� �� ֵ�����������һ����1��λ��
* ˵    ���� eg. �β�	 ����ֵ
				0x0001-->return 0
				0x0100-->return 2
*/
static rt_err_t get_eventkey(rt_uint32_t state)
{
	rt_uint8_t ret=0;
	for(int i=0;i<key_amount;i++)
	{
		if(state&(1<<i))
		{
			if(flag==0)
				ret=i;//���ذ����������е����
			flag++;
		}			
	}
	return ret;
}

/*
* �������ƣ�print_key
* ʵ    �Σ�*state-->���а���״̬�ı�����ַ
* �������ܣ�����ȷ��ȫ������״̬�ȶ�
* �� �� ֵ��RT_TRUE -->������״̬�ȶ��������а�������
			RT_FALSE-->״̬���ȶ��������������ɹ�������û�а�������
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
* ɨ�趨ʱ����ʱ����
*/
static void key_entry(void *parameter)
{
	static int mode = 5;//ɨ��ģʽ��0-->ȫ��ɨ��	1-->����ɨ��
	static rt_err_t press_num = 100;//��¼�¼�������������
	
	while(1)
	{
		if(print_key(&pin_status))//ɨ�����а���
		{
			if(pin_status!=0x0000)
				mode=0;
		}
		
		if(mode==2)
		{
			state =0; mode =5;
			switch(pin_status)
			{
				case 3:
					key_value[key_amount] = 1;
					break;
				case 5:
					key_value[key_amount] = 2;
					break;
				case 6:
					key_value[key_amount] = 3;
					break;
				default:
					break;
			}
		}
		
		if(mode==0)
		{
			press_num = get_eventkey(pin_status);//�õ��¼�����
			if(flag==1)
				mode=1;
			else if(flag>1)
				mode=2;
			flag =0;
		}
		else if(mode==1)
		{
			if(find_keyevent(press_num))//���������ʶ��
			{
				mode =5;
			}
		}		
		rt_thread_delay(scan_overtime);
	}
}

