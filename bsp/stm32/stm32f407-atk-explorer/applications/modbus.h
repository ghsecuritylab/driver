#ifndef __MODBUS_H__
#define __MODBUS_H__

#include <board.h>

typedef struct
{
	rt_uint8_t buffer[1024];
	rt_uint16_t index;
	rt_uint8_t report;
}rs485_t;

struct MODBUS_timer
{
	rt_device_t dev_timer;
	rt_hwtimerval_t timeout;
	rt_hwtimer_mode_t mode;
};

static rt_err_t rs485_write(rt_uint8_t *buffer,rt_uint16_t length);

#endif /* modbus.h */
