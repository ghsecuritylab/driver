#include "c_power.h"

static rt_device_t handle = RT_NULL;

static rt_uint32_t ai;
static rt_uint32_t di;
static rt_uint8_t a, b, c, d;
static rt_uint16_t e, f;

static void value_deal(void);

static void com_entry(void *parameter)
{
	lcd_show_string(0, 80, 24, "%s", " B A O H L T F P G U");
	lcd_show_string(0, 140, 24, "%s", " S  O  T  C  P  R  B");

	while(1) {
		rt_thread_mdelay(50);
		ai = rocker_get();
		di = button_get();
		a = (rt_uint8_t)(ai);
		b = (rt_uint8_t)(ai >> 8);
		c = (rt_uint8_t)(ai >> 16);
		d = (rt_uint8_t)(ai >> 24);
		e = (rt_uint16_t)(di);
		f = (rt_uint16_t)(di >> 16);
		lcd_show_string(0, 55, 24, " %03d %03d %03d %03d", c, d, a, b); //左履带 右履带 截割臂升降 截割臂转动
		value_deal();
	}
}

int community()
{
	rt_thread_t tid = RT_NULL;
	handle = rt_device_find("uart3");

	if(handle == RT_NULL) {
		rt_kprintf("usart3 device find failed!/n");
		return RT_ERROR;
	}

	rt_pin_mode(GET_PIN(D, 10), PIN_MODE_OUTPUT);
	rt_pin_write(GET_PIN(D, 10), PIN_HIGH); //使能发送
	tid = rt_thread_create("com", com_entry, RT_NULL, 1024, 7, 10);

	if(tid == RT_NULL) {
		rt_kprintf("com thread create failed!\n");
		return RT_ERROR;
	}

	rt_thread_startup(tid);
	rt_kprintf("com thread create success!\n");
	return RT_EOK;
}

static void value_deal(void)
{
	char k[19] = "0 0 0 0 0 0 0 0 0 0", m[19] = "0  0  0  0  0  0  0";

	if(e & (1 << OPC_BTN_ENBLE)) {
		k[0] = '1';
	}
	else {
		k[0] = '0';
	}

	if(e & (1 << OPC_AUTO_MODE)) {
		k[2] = '1';
	}
	else {
		k[2] = '0';
	}

	if(e & (1 << OPC_OILTANK_START)) {
		k[4] = '1';
	}
	else if(e & (1 << OPC_OILTANK_STOP)) {
		k[4] = '0';
	}

	if(e & (1 << OPC_HIGHCUT_START)) {
		k[6] = '1';
	}
	else if(e & (1 << OPC_HIGHCUT_STOP)) {
		k[6] = '0';
	}

	if(e & (1 << OPC_LOWCUT_START)) {
		k[8] = '1';
	}
	else if(e & (1 << OPC_LOWCUT_STOP)) {
		k[8] = '0';
	}

	if(e & (1 << OPC_TWOFUNC_START)) {
		k[10] = '1';
	}
	else if(e & (1 << OPC_TWOFUNC_STOP)) {
		k[10] = '0';
	}

	if(e & (1 << OPC_FAN_START)) {
		k[12] = '1';
	}
	else if(e & (1 << OPC_FAN_STOP)) {
		k[12] = '0';
	}

	if(e&(1<<OPC_PUMP_START)){
		k[14] = '1';
	}else if(e&(1<<OPC_PUMP_STOP)){
		k[14] = '0';
	}
	
	if(e & (1 << OPC_STUDY_MODE)) {                // OPC_STUDY_MODE
		k[16] = '1';
	}
	else {
		k[16] = '0';
	}

	if(e & (1 << OPC_SYSTEM_SHARP)) {                // OPC_SYSTEM_SHARP
		k[18] = '1';
	}
	else {
		k[18] = '0';
	}

	lcd_show_string(12, 105, 24, "%s", k);

	//动态开关处理
	if(f == 0) {
		lcd_show_string(0, 165, 24, "%s", " 0  0  0  0  0  0  0");
		return;
	}

	if(f & (1 << OPC_CUTARM_OUT)) {
		m[9] = '1';
	}
	else if(f & (1 << OPC_CUTARM_BACK)) {
		m[9] = '-';
		m[10] = '1';
	}

	if(f & (1 << OPC_STAR_FORWARD)) {
		m[0] = '1';
	}
	else if(f & (1 << OPC_STAR_REVERSE)) {
		m[0] = '-';
		m[1] = '1';
	}

	if(f & (1 << OPC_ONEFUNC_FORWARD)) {
		m[3] = '1';
	}
	else if(f & (1 << OPC_ONRFUNC_REVERSE)) {
		m[3] = '-';
		m[4] = '1';
	}

	if(f & (1 << OPC_RADAR_OUT)) {
		m[6] = '1';
	}
	else if(f & (1 << OPC_RADAR_BACK)) {
		m[6] = '-';
		m[7] = '1';
	}

	if(f & (1 << OPC_PLATE_UP)) {
		m[12] = '1';
	}
	else if(f & (1 << OPC_PLATE_DOWN)) {
		m[12] = '-';
		m[13] = '1';
	}

	if(f & (1 << OPC_REARPROP_UP)) {
		m[15] = '1';
	}
	else if(f & (1 << OPC_REARPROP_DOWN)) {
		m[15] = '-';
		m[16] = '1';
	}
	
	if(f & (1 << OPC_BELL_RING)) {  
		m[18] = '1';
	}
	lcd_show_string(12, 165, 24, "%s", m);
}
