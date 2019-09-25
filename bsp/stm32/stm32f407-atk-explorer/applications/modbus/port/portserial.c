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

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static variables ---------------------------------*/
static rt_device_t dev;//串口设备

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

static rt_err_t Rx_Cb(rt_device_t dev,rt_size_t size);//串口接收回调
/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	
	/*
	* xRxEnable --> 使能接收模式
	* xTxEnable --> 使能发送模式
	*/
	if(xRxEnable)
	{
		rt_pin_write(RS485,PIN_LOW);//485接收模式
	}
	else if(xTxEnable)
	{
		rt_pin_write(RS485,PIN_HIGH);//485发送模式
	}
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	/*
	* 串口功能初始化
	* 此处打开 uart2 串口设备
	*/
	struct serial_configure cfg = RT_SERIAL_CONFIG_DEFAULT;//先取默认参数
	rt_pin_mode(RS485,PIN_MODE_OUTPUT);//485使能引脚	
	dev = rt_device_find("uart2");
	if(dev==RT_NULL)
	{
		rt_kprintf("RS485串口查找失败!\n");
		return FALSE;
	}
	rt_device_open(dev,RT_DEVICE_FLAG_INT_RX);
	/*	配置参数	*/
	cfg.baud_rate = ulBaudRate;
	if(eParity)
		cfg.data_bits = ucDataBits + 1;//有校验，数据位数比指定数据位多1位
	else
		cfg.data_bits = ucDataBits;//无检验，数据位数根据指定位数设置
	cfg.parity = eParity;
	
	rt_device_control(dev,RT_DEVICE_CTRL_CONFIG,&cfg);//指定配置参数
	rt_device_set_rx_indicate(dev,Rx_Cb);//串口接收回调函数
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
	rt_device_write(dev,0,&ucByte,1);//串口发送一个字节数据
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
	rt_device_read(dev,0,pucByte,1);//从串口读取一个字节
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}


/*********************串口回调函数************************/
static rt_err_t Rx_Cb(rt_device_t dev,rt_size_t size)
{
	prvvUARTRxISR();//接收回调，调用FreeModbus 的接收中断函数
	return RT_EOK;
}

