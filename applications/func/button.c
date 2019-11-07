#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>

const rt_base_t btn_index[20] = {	// DI PIN INDEX BEGIN
	GET_PIN(E,14), GET_PIN(E,15), GET_PIN(E,12), GET_PIN(E,13),
	GET_PIN(E,10), GET_PIN(E,11), GET_PIN(E, 8), GET_PIN(E, 9),
	GET_PIN(B,12), GET_PIN(B,13), GET_PIN(B, 2), GET_PIN(E, 7),
	GET_PIN(B, 0), GET_PIN(B, 1), GET_PIN(C, 4), GET_PIN(C, 5),
	GET_PIN(A, 5), GET_PIN(A, 4), GET_PIN(D, 0), GET_PIN(A, 6)}; // END

#define J1_3	GET_PIN(E,14)	// 开关1_1
#define J1_4	GET_PIN(E,15)	// 开关1_3
#define J1_5	GET_PIN(E,12)	// 开关2_1
#define J1_6	GET_PIN(E,13)	// 开关2_3
#define J1_7	GET_PIN(E,10)	// 开关3_1
#define J1_8	GET_PIN(E,11)	// 开关3_3
	
#define J2_1	GET_PIN(E, 8)	// 开关4_1
#define J2_2	GET_PIN(E, 9)	// 开关4_3
#define J2_3	GET_PIN(B,12)	// 开关5_1
#define J2_4	GET_PIN(B,13)	// 开关5_3
#define J2_5	GET_PIN(B, 2)	// 开关6_1
#define J2_6	GET_PIN(E, 7)	// 开关6_3
#define J2_7	GET_PIN(B, 0)	// 开关7_1
#define J2_8	GET_PIN(B, 1)	// 开关7_3

#define J3_1	GET_PIN(C, 4)	// 开关8_1
#define J3_2	GET_PIN(C, 5)	// 开关8_3
#define J3_3	GET_PIN(A, 5)	// 开关9_1
#define J3_4	GET_PIN(A, 4)	// 开关9_3
#define J3_5	GET_PIN(D, 0)	// 开关10_1
#define J3_6	GET_PIN(A, 6)	// 开关10_3


static void btn_entry(void *parameter)
{
	while(1)
	{
		
		rt_thread_delay(100);
	}
}

void btn_init()
{
	
	for(int i=0;i<20;i++)
	{
		rt_pin_mode(btn_index[i],PIN_MODE_INPUT_PULLDOWN); // 下拉输入，默认低电平
	}
	
}
MSH_CMD_EXPORT(btn_init,btn pin and func init);
