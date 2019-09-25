/*
 * FreeModbus Libary: BARE Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4

#define REG_HOLDING_START 1000
#define REG_HOLDING_NREGS 4


/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputBuf[REG_INPUT_NREGS] = {0x0000,0x0001,0x0002,0x0003};
static USHORT	usRegHoldingBuf[REG_HOLDING_NREGS] = {0x0000,0x0001,0x0002,0x0003};

/* ----------------------- Start implementation -----------------------------*/
static void FreeModbus(void *parameter)
{
	while(1)
	{
		( void )eMBPoll(  );

		/* Here we simply count the number of poll cycles. */
		usRegInputBuf[0]++;
		rt_thread_delay(10);
	}
}

int modbus( void )
{
    eMBErrorCode    eStatus;

    eStatus = eMBInit( MB_RTU, 0x01, 0, 115200, MB_PAR_EVEN );

    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable(  );

	{
		rt_thread_t tid;
		tid = rt_thread_create("Free",FreeModbus,RT_NULL,1024,10,20);
		if(tid!=RT_NULL)
			rt_thread_startup(tid);
	}
	return 0;
}
INIT_APP_EXPORT(modbus);

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;	//初始化返回值，无错误
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )	//地址与线圈数量符合设定值
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - REG_INPUT_START );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );	//先返回高字节
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF ); //后返回低字节
            iRegIndex++;
            usNRegs--;	//每个线圈需要返回两个字节数据
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;	//正常--> MB_ENOERR ; 错误--> MB_ENOREG
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;	//初始化返回值，无错误
    int             hRegIndex;

    if( ( usAddress >= REG_HOLDING_START )	//地址与线圈数量符合设定值
        && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        hRegIndex = ( int )( usAddress - REG_HOLDING_START );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegHoldingBuf[hRegIndex] >> 8 ); //先返回高字节
            *pucRegBuffer++ =
                ( unsigned char )( usRegHoldingBuf[hRegIndex] & 0xFF ); //后返回低字节
            hRegIndex++;
            usNRegs--;	//每个线圈需要返回两个字节数据
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus; //正常--> MB_ENOERR ; 错误--> MB_ENOREG
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
