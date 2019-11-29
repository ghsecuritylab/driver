#include "c_power.h"

#define DBG_ENABLE
#define DBG_SECTION_NAME  "drv.button"
#define DBG_LEVEL          DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

static rt_uint16_t steady = 0x2aa8;// ���濪��
static rt_uint16_t action = 0x0000;// ��̬����

#define btn_num      29      // ��������
static rt_mutex_t btn_lock = RT_NULL;
static rt_uint32_t btn_list = 0x00;

const rt_base_t btn_index[btn_num] = {// DI PIN INDEX BEGIN
    GET_PIN(E, 1), GET_PIN(B,11), GET_PIN(E, 0), GET_PIN(D, 5),// DI0 DI30 DI2 DI3
    GET_PIN(B, 9), GET_PIN(D, 4), GET_PIN(B, 8), GET_PIN(D, 3),// DI4 ~ DI7
    GET_PIN(A, 7), GET_PIN(D, 1), GET_PIN(A, 6), GET_PIN(D, 0),// DI8 ~ DI11
    GET_PIN(A, 4), GET_PIN(A, 5),                              // DI12~DI13
    GET_PIN(B, 0), GET_PIN(E, 7), GET_PIN(B, 2), GET_PIN(B,13),// DI17~DI20
    GET_PIN(B,12), GET_PIN(E, 9), GET_PIN(E, 8), GET_PIN(E,11),// DI21~DI24
    GET_PIN(E,10), GET_PIN(E,13), GET_PIN(E,12), GET_PIN(E,15),// DI25~DI28
	GET_PIN(B,10), 											   // DI31
	GET_PIN(E,14), 											   // DI29
	GET_PIN(D, 6)				   							   // DI1
};// END

/*
* get moudle data
*/
rt_uint32_t button_get(void)
{
    rt_uint32_t resault;
    rt_mutex_take(btn_lock,RT_WAITING_FOREVER);
    resault = ((action<<16)|(steady));
    rt_mutex_release(btn_lock);
    return resault;
}

/*
* scan all buttons defined in btn_index
*/
static rt_uint32_t btn_get()
{
    rt_uint32_t status = 0x00000000;
    for(int i=0; i<btn_num; i++) {
        status |= ((rt_pin_read(btn_index[i])) << i);
    }
    return status;
}

/*
* deal with btn event
*/
static void btn_process()
{
    rt_uint32_t res = btn_get();
    if(btn_list == res) {
		
		//ϵͳ��ͣ����������Ĭ��״̬����12V
		if(!(btn_list & (1<<26))){//ϵͳ��ͣ
			steady = 0x2aa8 | (1<<OPC_SYSTEM_SHARP);//��̬����Ĭ��״̬ + ��ͣ״̬
			action = 0x0000;//��̬��������
			return;
		}
		
		//����ʹ�ܿ���˫�����������м�̬
		if(btn_list & (1<<0)) {// ����ʹ��λ
			steady &= (0xffff^(1<<OPC_SYSTEM_SHARP));  //�����ͣ״̬λ
			steady |= (1<<OPC_BTN_ENBLE);        	  // set bit0 1
		}else{
			steady = 0x2aa8;
			action = 0x0000;
			return;
		}
		
		// �Կ���ѧϰ���⣬˫���������ؿ��ƣ����м�״̬
        if(btn_list & (1<<1)) {// �Կ�
            steady |= (1<<OPC_AUTO_MODE);             // set bit1  1
			steady &= (0xffff^(1<<OPC_STUDY_MODE));	  // set bit14 0
        }else if(btn_list&(1<<27)){// ѧϰģʽ
			steady &= (0xffff^(1<<OPC_AUTO_MODE));	  // set bit1  0
			steady |= (1<<OPC_STUDY_MODE);			  // set bit14 1
		}else{
			steady &= (0xffff^(1<<OPC_AUTO_MODE));	  // set bit1  0
			steady &= (0xffff^(1<<OPC_STUDY_MODE));	  // set bit14 0
		}

		//��ͣ����˫����������м�̬
        if(btn_list & (1<<2)) {// �ͱ�
            steady |= (1<<OPC_OILTANK_START);         // set bit2 1
            steady &= (0xffff^(1<<OPC_OILTANK_STOP)); // set bit3 0
        } else if(btn_list & (1<<3)) {
            steady &= (0xffff^(1<<OPC_OILTANK_START));// set bit2 0
            steady |= (1<<OPC_OILTANK_STOP);          // set bit3 1
        }

        if(btn_list & (1<<4)) {// �ظ�
            steady |= (1<<OPC_HIGHCUT_START);         // set bit4 1
            steady &= (0xffff^(1<<OPC_HIGHCUT_STOP)); // set bit5 0
        } else if(btn_list & (1<<5)) {
            steady &= (0xffff^(1<<OPC_HIGHCUT_START));// set bit4 0
            steady |= (1<<OPC_HIGHCUT_STOP);          // set bit5 1
        }

        if(btn_list & (1<<6)) {// �ص�
            steady |= (1<<OPC_LOWCUT_START);          // set bit6 1
            steady &= (0xffff^(1<<OPC_LOWCUT_STOP));  // set bit7 0
        } else if(btn_list & (1<<7)) {
            steady &= (0xffff^(1<<OPC_LOWCUT_START)); // set bit6 0
            steady |= (1<<OPC_LOWCUT_STOP);           // set bit7 1
        }

        if(btn_list & (1<<8)) {// ������ͣ
            steady |= (1<<OPC_TWOFUNC_START);         // set bit8 1
            steady &= (0xffff^(1<<OPC_TWOFUNC_STOP)); // set bit9 0
        } else if(btn_list & (1<<9)) {
            steady &= (0xffff^(1<<OPC_TWOFUNC_START));// set bit8 0
            steady |= (1<<OPC_TWOFUNC_STOP);          // set bit9 1
        }

        if(btn_list & (1<<10)) {// ���
            steady |= (1<<OPC_FAN_START);             // set bit10 1
            steady &= (0xffff^(1<<OPC_FAN_STOP));     // set bit11 0
        } else if(btn_list & (1<<11)) {
            steady &= (0xffff^(1<<OPC_FAN_START));    // set bit10 1
            steady |= (1<<OPC_FAN_STOP);              // set bit11 0
        }

        if(btn_list & (1<<20)) {// ˮ��
            steady |= (1<<OPC_PUMP_START);            // set bit12 1
            steady &= (0xffff^(1<<OPC_PUMP_STOP));    // set bit13 0
        } else if(btn_list & (1<<21)) {
            steady &= (0xffff^(1<<OPC_PUMP_START));   // set bit12 0
            steady |= (1<<OPC_PUMP_STOP);             // set bit13 1
        }

        action = 0x0000;// ���㶯̬��������
        if(btn_list & (1<<12)) {// �ظ��
            action |= (1<<OPC_CUTARM_OUT);            // set bit0 1
        } else if(btn_list & (1<<13)) {
            action |= (1<<OPC_CUTARM_BACK);           // set bit1 1
        }

        if(btn_list & (1<<14)) {// ����
            action |= (1<<OPC_STAR_FORWARD);          // set bit2 1
        } else if(btn_list & (1<<15)) {
            action |= (1<<OPC_STAR_REVERSE);          // set bit3 1
        }

        if(btn_list & (1<<16)) {// һ��ת��
            action |= (1<<OPC_ONEFUNC_FORWARD);       // set bit4 1
        } else if(btn_list & (1<<17)) {
            action |= (1<<OPC_ONRFUNC_REVERSE);       // set bit5 1
        }

        if(btn_list & (1<<18)) {// ����ת��
            action |= (1<<OPC_RADAR_OUT);       	  // set bit6 1
        } else if(btn_list & (1<<19)) {
            action |= (1<<OPC_RADAR_BACK);       	  // set bit7 1
        }

        if(btn_list & (1<<22)) {// ����
            action |= (1<<OPC_PLATE_UP);              // set bit8 1
        } else if(btn_list & (1<<23)) {
            action |= (1<<OPC_PLATE_DOWN);            // set bit9 1
        }

        if(btn_list & (1<<24)) {// ��֧��
            action |= (1<<OPC_REARPROP_UP);           // set bit10 1
        } else if(btn_list & (1<<25)) {
            action |= (1<<OPC_REARPROP_DOWN);         // set bit11 1
        }
		
		if(btn_list&(1<<28)){// ����
			action |= (1<<OPC_BELL_RING);			  // set bit12 1
		}
    } else {
        btn_list = res;// refresh btn_list value
    }
}

static void btn_entry(void *parameter)
{
    while(1)
    {
        rt_mutex_take(btn_lock,RT_WAITING_FOREVER);
        btn_process();
        rt_mutex_release(btn_lock);

        rt_thread_delay(15);
    }
}

int hw_button_init(void)
{
    for(int i=0; i<btn_num; i++)
    {
        rt_pin_mode(btn_index[i],PIN_MODE_INPUT_PULLDOWN); //Ĭ�ϵ͵�ƽ��������������
    }

    btn_lock = rt_mutex_create("btn_lock",RT_IPC_FLAG_FIFO);
    if(btn_num==RT_NULL)
    {
        rt_free(btn_lock);
        LOG_E("btn_lock create failed!");
        return -RT_ERROR;
    }

    {
        rt_thread_t tid = RT_NULL;
        tid = rt_thread_create(BUTTON_THREAD_NAME,btn_entry,RT_NULL,
                               BUTTON_THREAD_STACK_SIZE,BUTTON_THREAD_PRIORITY,20);
        if(tid==RT_NULL)
        {
            rt_free(tid);
            rt_free(btn_lock);
            LOG_E("btn thread create failed!");
            return -RT_ERROR;
        }
        rt_thread_startup(tid);
    }
    LOG_I("button init success!");
    return RT_EOK;
}
