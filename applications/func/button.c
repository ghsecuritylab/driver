#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>

const rt_base_t btn_index[20] = {	// DI PIN INDEX BEGIN
	GET_PIN(E,14), GET_PIN(E,15), GET_PIN(E,12), GET_PIN(E,13),
	GET_PIN(E,10), GET_PIN(E,11), GET_PIN(E, 8), GET_PIN(E, 9),
	GET_PIN(B,12), GET_PIN(B,13), GET_PIN(B, 2), GET_PIN(E, 7),
	GET_PIN(B, 0), GET_PIN(B, 1), GET_PIN(C, 4), GET_PIN(C, 5),
	GET_PIN(A, 5), GET_PIN(A, 4), GET_PIN(D, 0), GET_PIN(A, 6),
}; // END


static void btn_entry(void *parameter)
{
	rt_uint32_t status = 0x00;
	while(1)
	{
		for(int i=0;i<20;i++)
		{
			if(rt_pin_read(btn_index[i]);
				status |= 1<<i;	// 位标识开关动作
		}
		
		if(status)
			rt_kprintf("%b\n",status);
		status = 0x0000;
		rt_thread_delay(10);
	}
}

void btn_init()
{
	
	for(int i=0;i<20;i++)
	{
		rt_pin_mode(btn_index[i],PIN_MODE_INPUT_PULLDOWN); //默认低电平，配置下拉输入
	}
	
	{
		rt_thread_t tid = RT_NULL;
		tid = rt_thread_create("button",btn_entry,RT_NULL,1024,8,20);
		if(tid==RT_NULL)
		{
			rt_kprintf("btn thread create failed!\n");
			return;
		}
		rt_thread_startup(tid);
	}
}
MSH_CMD_EXPORT(btn_init,btn pin and func init);
