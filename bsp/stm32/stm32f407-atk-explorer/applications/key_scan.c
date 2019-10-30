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

static 	rt_timer_t scan = RT_NULL;//��ʱ�����

static rt_uint8_t key_value[key_amount+1]={0};//�������ܴ洢����

static rt_uint32_t pin_status = 0;//��ǰ��������״̬��32λ��ʶ32������
static rt_err_t state=0;//״̬��״̬
struct key_struct key_creat =
{
	.parent = 0,
	.name = key_device_name
};//key �豸

/*
* ���а��������ų�ʼ��
* ������� *pin �����а����ŵ������׵�ַ
*/
static void stm32_key_gpio_init(rt_uint8_t *pin)
{
	rt_uint8_t *p = RT_NULL;//��������ָ��
	p = pin;//ȡ�����������׵�ַ
	for(int i=0;i<key_amount;i++)
	{
		rt_pin_mode(*p,PIN_MODE_INPUT_PULLUP);
		p++;//ָ����һ��������
	}
}

/*
* �������ƣ�stm32_key_init
* �������ܣ����� key ���ܵĶ�ʱ��
* ���ز����������ɹ� --> RT_EOK
			����ʧ�� --> RT_ERROR
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
* �������ƣ�stm32_key_start
* ���������oflag	1-->����; 0-->�޲���
* �������ܣ�����ɨ�趨ʱ��
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
* �������ƣ�stm32_key_stop
* �������ܣ�ֹͣɨ�趨ʱ��
*/
static rt_err_t stm32_key_close(struct rt_device *dev)
{
	RT_ASSERT(dev!=RT_NULL);
	
	rt_timer_stop(scan);
	
	return RT_EOK;
}

/*
* �������ƣ�stm32_key_read
* ���������pos		���λ��
			*buffer ������а���״̬�Ļ�������
			size 	�ܶ�ȡλ��
* �������ܣ���32������״̬��ȡ��ָ������
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
* �������ƣ�rt_hw_key_init
* �������ܣ�ע�� key �����豸
* ����˵�������ⲿ����
*/
rt_err_t rt_hw_key_init(void)
{	
	stm32_key_gpio_init(pin_list);
	
	key_creat.parent.type = RT_Device_Class_Miscellaneous;
	key_creat.parent.rx_indicate = RT_NULL;
	key_creat.parent.tx_complete = RT_NULL;
	
	/*		��������		*/
	key_creat.parent.init = stm32_key_init;
	key_creat.parent.open = stm32_key_open;
	key_creat.parent.close = stm32_key_close;
	key_creat.parent.read = stm32_key_read;
	key_creat.parent.control = RT_NULL;
	key_creat.parent.write = RT_NULL;
	
	/*		˽������		*/
	key_creat.parent.user_data = RT_NULL;
		
	return rt_device_register(&key_creat.parent,key_creat.name,RT_DEVICE_FLAG_RDONLY);
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
			if(!rt_pin_read(pin_list[num]))//������
			{
				state = 1;
				press_time = 0;
			}
		break;
			
		case 1://�����£�����ɿ����ǳ���
			if(rt_pin_read(pin_list[num]))//�������ͷ�
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
			if(rt_pin_read(pin_list[num]))
			{
				key_return = KEY_LONG;//���س�������
				if(!key_value[num])
					key_value[num]=3;
				state = 0;
			}
		break;
		
		case 3://˫��ʶ��
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
		if(rt_pin_read(pin_list[i])==0)//�����Ӧ����������
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
	for(int i=0;i<key_amount;i++)
	{
		if(state&(1<<i))
			return i;//���ذ����������е����
	}
	return RT_FALSE;
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
static void scan_entry(void *parameter)
{
	static int mode = 0;//ɨ��ģʽ��0-->ȫ��ɨ��	1-->����ɨ��
	static rt_err_t press_num = 0;//��¼�¼�������������
	
	if(pin_status==0)
		mode=0;
	
	if(mode==0)//ģʽ0��ɨ�����а���
	{
		if(print_key(&pin_status))
		{
			mode=1;
			press_num = get_eventkey(pin_status);//�õ��¼�����
		}
	}
	else if(mode==1)
	{
		if(find_keyevent(press_num))//���������ʶ��
		{
			mode =0;
		}	
	}
}
