#include "c_power.h"

#define DBG_ENABLE 
#define DBG_SECTION_NAME  "drv.rocker"
#define DBG_LEVEL          DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

static AD7739_t rocker = RT_NULL;
static rt_sem_t rocker_lock = RT_NULL;
static rt_mutex_t mess_lock = RT_NULL;

#define ch_amount    4 //ad7739 读取通道数
static rt_uint8_t AI_Data[ch_amount] = {128,128,128,128};

/*
* get rocker's data
*/
rt_uint32_t rocker_get(void)
{
	rt_uint32_t resault = 0;	
	rt_mutex_take(mess_lock,RT_WAITING_FOREVER);
	resault = ((AI_Data[3]<<24)|(AI_Data[2]<<16)|(AI_Data[1]<<8)|(AI_Data[0]));
	rt_mutex_release(mess_lock);
	return resault;
}

/*
* translate value between 0 and 255
*/
static rt_uint8_t deal(rt_uint32_t value)
{
    float trans = 0;

	// limit max and min
    if(value>450)
        value = 450;
    else if(value<50)
        value = 50;

    // resort range of value
    if((value<260)&&(value>240))
        return 128;
    else if(value>250)
    {
        trans = (value-250.0) / 200.0;
        return (rt_uint8_t)(127+128*trans);
    }
    else if(value<250)
    {
        trans = (250.0-value) / 200.0;
        return (rt_uint8_t)(128-128*trans);
    }
    else
        return 128;
}

/*
* read and deal AD7739 data
*/
static void ad_process()
{
    float temp = 0;
    rt_uint8_t data[ch_amount*3] = {0};
    static rt_uint32_t rocker_buffer[ch_amount] = {0};
    static rt_uint8_t flag = 0;

    flag++;

    ad7739_channel_read(rocker,data,3);// read all enabled channel
    for(int i=0; i<ch_amount; i++) // link all enabled AD channels data
    {
        int j=i*3;
        rocker_buffer[i] += (data[j]<<16) | (data[j+1]<<8) | (data[j+2]);
    }

    // deal with data after five read ops
    if(flag == 5)
    {
        flag = 0;
        for(int i=0; i<4; i++)
        {
            // using 1.0 replace 5.0 because of filtering
            temp = rocker_buffer[i] * 1.0 / (1<<24);
            rocker_buffer[i] = (rt_uint32_t)(temp * 100);
        }

        rt_memset(AI_Data,128,sizeof(AI_Data));// default value

        if(rt_pin_read(Clip_Hand))
        {
            rt_thread_delay(10);
            if(rt_pin_read(Clip_Hand))
            {
                AI_Data[0] = deal(rocker_buffer[0]);
                AI_Data[1] = deal(rocker_buffer[1]);
            }
        }
        if(rt_pin_read(Left_Wheel))
        {
            rt_thread_delay(10);
            if(rt_pin_read(Left_Wheel))
            {
                AI_Data[2] = deal(rocker_buffer[2]);
            }
        }
        if(rt_pin_read(Right_Wheel))
        {
            rt_thread_delay(10);
            if(rt_pin_read(Right_Wheel))
            {
                AI_Data[3] = deal(rocker_buffer[3]);
            }
        }

        rt_memset(rocker_buffer,0,sizeof(rocker_buffer));
    }
}

/*
* AD7739 ready pin irq callback func
*/
static void ready(void *args)
{
    rt_sem_release(rocker_lock);// release semaphore
}

/*
* rocker thread func
*/
static void rocker_entry(void *parameter)
{
    while(1)
    {
        rt_sem_take(rocker_lock,RT_WAITING_FOREVER);// waiting for AD7739 translate over
        rt_pin_irq_enable(rocker->ready_pin,PIN_IRQ_DISABLE);// stop AD7739 ready pin irq

        rt_mutex_take(mess_lock,RT_WAITING_FOREVER);
        ad_process();
		rt_mutex_release(mess_lock);

        rt_thread_mdelay(10);// drop out cpu

        rt_pin_irq_enable(rocker->ready_pin,PIN_IRQ_ENABLE);// restart ready pin irq
    }
}

int hw_rocker_init(void)
{
    rt_thread_t tid = RT_NULL;
    rocker = rt_calloc(1,sizeof(struct AD7739));
    if(rocker==RT_NULL)
    {
        LOG_E("rocker device memory apply failed!\n");
        ad7739_deinit(rocker);
        return -RT_ERROR;
    }

    rocker->adc_device_name = "rocker_device";
    rocker->channel_enable = 0x0f;
    rocker->ready_pin = GET_PIN(B, 7);
    rocker->reset_pin = GET_PIN(B, 6);
    ad7739_init("spi1",rocker,GPIOB,GPIO_PIN_5);

    rocker_lock = rt_sem_create("rocker_lock",0,RT_IPC_FLAG_FIFO);
    if(rocker_lock == RT_NULL)
    {
        LOG_E("rocker_lock create falied!\n");
        rt_free(rocker_lock);
        ad7739_deinit(rocker);
        return -RT_ERROR;
    }

    tid = rt_thread_create(ROCKER_THREAD_NAME,rocker_entry,RT_NULL,
							ROCKER_THREAD_STACK_SIZE,ROCKER_THREAD_PRIORITY,15);
    if(tid==RT_NULL)
    {
        LOG_E("rocker thread create failed!\n");
        rt_free(rocker_lock);
        ad7739_deinit(rocker);
        return -RT_ERROR;
    }
    
	mess_lock = rt_mutex_create("rocker_mess_lock",RT_IPC_FLAG_FIFO);
    if(mess_lock == RT_NULL)
    {
        LOG_E("rocker mess_lock create falied!\n");
        rt_free(rocker_lock);
		rt_free(mess_lock);
        ad7739_deinit(rocker);
        return -RT_ERROR;
    }
	
    LOG_I("rocker init success!/n");

    rt_pin_attach_irq(rocker->ready_pin,PIN_IRQ_MODE_FALLING,ready,RT_NULL);//bind ready pin irq
    rt_pin_irq_enable(rocker->ready_pin,PIN_IRQ_ENABLE);// enable irq

    rt_pin_mode(Left_Wheel,PIN_MODE_INPUT_PULLDOWN);
    rt_pin_mode(Right_Wheel,PIN_MODE_INPUT_PULLDOWN);
    rt_pin_mode(Clip_Hand,PIN_MODE_INPUT_PULLDOWN);

    rt_thread_startup(tid);
    
    return RT_EOK;
}
