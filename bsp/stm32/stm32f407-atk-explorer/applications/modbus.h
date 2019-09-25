#ifndef __MODBUS_H__
#define __MODBUS_H__

#include <board.h>

#define RS485	GET_PIN(G, 8)

#define	SLAVE	0x01	//从机站号
#define Message_MAX	512	//定义一帧数据接收最大字节数

#define Ack_Ok		0x00
#define Error_Cmd	0x01
#define Error_Value	0x02

typedef struct
{
	rt_uint8_t RxBuffer[Message_MAX];
	rt_uint16_t RxIndex;
	rt_uint8_t TxBuffer[Message_MAX];
	rt_uint16_t TxIndex;
	
	rt_uint8_t Rsp;
}rs485_t;

struct MODBUS_timer
{
	rt_device_t dev_timer;
	rt_hwtimerval_t timeout;
	rt_hwtimer_mode_t mode;
};

static rt_err_t rs485_write(rt_uint8_t *buffer,rt_uint16_t length);
static rt_uint16_t crc_modbus(rt_uint8_t *buffer,rt_uint8_t size);

static void Send_WithCrc(rt_uint8_t *buffer,rt_uint8_t length);
static void Send_ErrorAck(rt_uint8_t ack);
static void Modbus_01H(void);
static void Modbus_04H(void);

#endif /* modbus.h */
