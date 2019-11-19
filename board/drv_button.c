#include "c_power.h"

#define DBG_ENABLE 
#define DBG_SECTION_NAME  "drv.button"
#define DBG_LEVEL          DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

/*****开关接口数据*****/
static rt_uint16_t steady = 0x0000;// 锁存开关
static rt_uint16_t action = 0x0000;// 动态开关


#define btn_num		26		// 开关数量
static rt_mutex_t btn_lock = RT_NULL;
static rt_uint32_t btn_list = 0x00;
static rt_bool_t enable = 0;

const rt_base_t btn_index[btn_num] = {// DI PIN INDEX BEGIN
    GET_PIN(E, 1), GET_PIN(D, 6), GET_PIN(E, 0), GET_PIN(D, 5),// DI0 ~ DI3
    GET_PIN(B, 9), GET_PIN(D, 4), GET_PIN(B, 8), GET_PIN(D, 3),// DI4 ~ DI7
    GET_PIN(A, 7), GET_PIN(D, 1), GET_PIN(A, 6), GET_PIN(D, 0),// DI8 ~ DI11
    GET_PIN(A, 4), GET_PIN(A, 5),                              // DI12~DI13
    GET_PIN(B, 0), GET_PIN(E, 7), GET_PIN(B, 2), GET_PIN(B,13),// DI17~DI20
    GET_PIN(B,12), GET_PIN(E, 9), GET_PIN(E, 8), GET_PIN(E,11),// DI21~DI24
    GET_PIN(E,10), GET_PIN(E,13), GET_PIN(E,12), GET_PIN(E,15),// DI25~DI28

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
    rt_uint32_t status = 0x00;
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
        if(!enable) {
            if(btn_list & (1<<0)) { // 控制使能
                enable = 1;
                steady |= (1<<btn_enable);        // set bit0 1
            }
            return;
        }
        if(btn_list & (1<<1)) {
            steady |= (1<<auto_mode);             // set bit1 1
        }

        if(btn_list & (1<<2)) {// 油泵
            steady |= (1<<oiltank_start);		  // set bit2 1
            steady &= (0xffff^(1<<oiltank_stop)); // set bit3 0
        } else if(btn_list & (1<<3)) {
            steady &= (0xffff^(1<<oiltank_start));// set bit2 0
            steady |= (1<<oiltank_stop);          // set bit3 1
        }

        if(btn_list & (1<<4)) {// 截高
            steady |= (1<<highcut_start);         // set bit4 1
            steady &= (0xffff^(1<<haghcut_stop)); // set bit5 0
        } else if(btn_list & (1<<5)) {
            steady &= (0xffff^(1<<highcut_start));// set bit4 0
            steady |= (1<<haghcut_stop);          // set bit5 1
        }

        if(btn_list & (1<<6)) {// 截低
            steady |= (1<<lowcut_start);          // set bit6 1
            steady &= (0xffff^(1<<lowcut_stop));  // set bit7 0
        } else if(btn_list & (1<<7)) {
            steady &= (0xffff^(1<<lowcut_start)); // set bit6 0
            steady |= (1<<lowcut_stop);           // set bit7 1
        }

        if(btn_list & (1<<8)) {// 二运启停
            steady |= (1<<twofunc_start);         // set bit8 1
            steady &= (0xffff^(1<<twofunc_stop)); // set bit9 0
        } else if(btn_list & (1<<9)) {
            steady &= (0xffff^(1<<twofunc_start));// set bit8 0
            steady |= (1<<twofunc_stop);          // set bit9 1
        }

        if(btn_list & (1<<10)) {// 风机
            steady |= (1<<fan_start);             // set bit10 1
            steady &= (0xffff^(1<<fan_stop));     // set bit11 0
        } else if(btn_list & (1<<11)) {
            steady &= (0xffff^(1<<fan_start));    // set bit10 1
            steady |= (1<<fan_stop);              // set bit11 0
        }
		
		if(btn_list & (1<<20)) { // 水泵
            steady |= (1<<pump_start);            // set bit12 1
			steady &= (0xffff^(1<<pump_stop));    // set bit13 0
        } else if(btn_list & (1<<21)) {
            steady &= (0xffff^(1<<pump_start));   // set bit12 0
            steady |= (1<<pump_stop);             // set bit13 1
        }
		

        action = 0x0000;// 清零动态操作开关
        if(btn_list & (1<<12)) {// 截割臂
            action |= (1<<cutarm_out);            // set bit0 1
        } else if(btn_list & (1<<13)) {
            action |= (1<<cutarm_back);	          // set bit1 1
        }

        if(btn_list & (1<<14)) {// 星轮
            action |= (1<<star_forward);          // set bit2 1
        } else if(btn_list & (1<<15)) {
            action |= (1<<star_reverse);          // set bit3 1
        }

        if(btn_list & (1<<16)) { // 一运转动
            action |= (1<<onefunc_forward);       // set bit4 1
        } else if(btn_list & (1<<17)) {
            action |= (1<<onefunc_reverse);       // set bit5 1
        }

        if(btn_list & (1<<18)) { // 二运转动
            action |= (1<<twofunc_forward);       // set bit6 1
        } else if(btn_list & (1<<19)) {
            action |= (1<<twofunc_reverse);       // set bit7 1
        }

        if(btn_list & (1<<22)) { // 铲板
            action |= (1<<plate_up);              // set bit8 1
        } else if(btn_list & (1<<23)) {
            action |= (1<<plate_down);            // set bit9 1
        }

        if(btn_list & (1<<24)) { // 后支撑
            action |= (1<<rearprop_up);           // set bit10 1
        } else if(btn_list & (1<<25)) {
            action |= (1<<rearprop_down);         // set bit11 1
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
	for(int i=0; i<20; i++)
    {
        rt_pin_mode(btn_index[i],PIN_MODE_INPUT_PULLDOWN); //默认低电平，配置下拉输入
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
        tid = rt_thread_create("button",btn_entry,RT_NULL,512,11,20);
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
