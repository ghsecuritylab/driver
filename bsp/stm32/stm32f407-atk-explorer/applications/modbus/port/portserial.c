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
static rt_device_t dev;//�����豸

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

static rt_err_t Rx_Cb(rt_device_t dev,rt_size_t size);//���ڽ��ջص�
/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	
	/*
	* xRxEnable --> ʹ�ܽ���ģʽ
	* xTxEnable --> ʹ�ܷ���ģʽ
	*/
	if(xRxEnable)
	{
		rt_pin_write(RS485,PIN_LOW);//485����ģʽ
	}
	else if(xTxEnable)
	{
		rt_pin_write(RS485,PIN_HIGH);//485����ģʽ
	}
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	/*
	* ���ڹ��ܳ�ʼ��
	* �˴��� uart2 �����豸
	*/
	struct serial_configure cfg = RT_SERIAL_CONFIG_DEFAULT;//��ȡĬ�ϲ���
	rt_pin_mode(RS485,PIN_MODE_OUTPUT);//485ʹ������	
	dev = rt_device_find("uart2");
	if(dev==RT_NULL)
	{
		rt_kprintf("RS485���ڲ���ʧ��!\n");
		return FALSE;
	}
	rt_device_open(dev,RT_DEVICE_FLAG_INT_RX);
	/*	���ò���	*/
	cfg.baud_rate = ulBaudRate;
	if(eParity)
		cfg.data_bits = ucDataBits + 1;//��У�飬����λ����ָ������λ��1λ
	else
		cfg.data_bits = ucDataBits;//�޼��飬����λ������ָ��λ������
	cfg.parity = eParity;
	
	rt_device_control(dev,RT_DEVICE_CTRL_CONFIG,&cfg);//ָ�����ò���
	rt_device_set_rx_indicate(dev,Rx_Cb);//���ڽ��ջص�����
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
	rt_device_write(dev,0,&ucByte,1);//���ڷ���һ���ֽ�����
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
	rt_device_read(dev,0,pucByte,1);//�Ӵ��ڶ�ȡһ���ֽ�
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


/*********************���ڻص�����************************/
static rt_err_t Rx_Cb(rt_device_t dev,rt_size_t size)
{
	prvvUARTRxISR();//���ջص�������FreeModbus �Ľ����жϺ���
	return RT_EOK;
}

