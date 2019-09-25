/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static variables ---------------------------------*/
static struct MODBUS_timer RS485_timer;//定时器结构体

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

static rt_err_t timer_out(rt_device_t dev,rt_size_t size);

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	RS485_timer.mode = HWTIMER_MODE_ONESHOT;
	RS485_timer.timeout.sec = 0;
	RS485_timer.timeout.usec = usTim1Timerout50us * 50;
	RS485_timer.dev_timer = rt_device_find("timer13");
	if(RS485_timer.dev_timer==RT_NULL)
	{
		rt_kprintf("timer13 find error!\n");
		return FALSE;
	}
	if(rt_device_open(RS485_timer.dev_timer,RT_DEVICE_OFLAG_RDWR) != RT_EOK)
		rt_kprintf("timer13 open error!\n");
	if(rt_device_control(RS485_timer.dev_timer,HWTIMER_CTRL_MODE_SET,&RS485_timer.mode))
		rt_kprintf("timer13 mode set error!\n");
	rt_device_set_rx_indicate(RS485_timer.dev_timer,timer_out);//定时器超时回调函数
    return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
	rt_device_write(RS485_timer.dev_timer,0,&RS485_timer.timeout,sizeof(RS485_timer.timeout));//启动定时
}

inline void
vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}

/**************回调函数************/
static rt_err_t timer_out(rt_device_t dev,rt_size_t size)
{
	prvvTIMERExpiredISR();//定时器超时，调用FreeModbus定时中断
	return RT_EOK;
}

