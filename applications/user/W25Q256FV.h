#ifndef __W25Q256FV_H__
#define __W25Q256FV_H__

#include <board.h>
#include <rtthread.h>

#define Sector_Total	8192	//W25Q256FV 扇区序号最大值，origin from 0

/* ==== Addr range (four Byte) ==== */
#define addr_min 0x00000000
#define addr_max 0x01FFFFFF

/* ========== Device  ID ========== */
#define MF_ID			(0xEF)		// Manufacturer id
#define DEVICE_ID		(0x18)		// ID7-ID0
#define W25Q256FV_SPI	(0x4019)	// SPI Mode  ID15-ID0
#define W25Q256FV_QSPI	(0x6019)	// QSPI Mode ID15-ID0 

/* =========== Cmd list =========== */
#define	W25Q_WREN	(0x06)			// Write Enable
#define W25Q_WRDI	(0x04)			// Write Disable
#define	W25Q_RDSR1	(0x05)			// Read Status Register1
#define W25Q_WRSR1	(0x01)			// Write Status Register1
#define	W25Q_RDSR2	(0x35)			// Read Status Register2
#define	W25Q_WRSR2	(0x31)			// Write Status Register2
#define	W25Q_RDSR3	(0x15)			// Read Status Register3
#define	W25Q_WRSR3	(0x11)			// Write Status Register3
#define	W25Q_CERA	(0xc7)			// Chip Erase
#define W25Q_SEER	(0x20)			// Sector Erase 4k
#define W25Q_BLER	(0x52)			// Block Erase 32k
#define	W25Q_MFID	(0x90)			// Manufacturer ID
#define	W25Q_JEID	(0x9f)			// JEDEC ID
#define	W25Q_EN4B	(0xb7)			// Enter 4-Byte Address Mode
#define	W25Q_EX4B	(0xe9)			// Exit 4-Byte Address Mode
#define	W25Q_READ	(0x13)			// Read Data with 4-Byte Address
#define W25Q_WRPA	(0x02)			// Page Program
#define W25Q_ENRE	(0x66)			// Enable Reset
#define W25Q_REDE	(0x99)			// Reset Device

typedef enum 	//default case: TB= 0; WPS = 0; CMP = 0
{
	W25Q_NONE	= 0x00,				// NONE
	W25Q_TOP1	= 0x04,				// 01FF0000h - 01FFFFFFh,64K  Upper 1/512
	W25Q_TOP2	= 0x08,				// 01FE0000h - 01FFFFFFh,128K Upper 1/256
	W25Q_TOP4	= 0x0C,				// 01FC0000h - 01FFFFFFh,256K Upper 1/128
	W25Q_TOP8	= 0x10,				// 01F80000h - 01FFFFFFh,512K Upper 1/64
	W25Q_TOP16	= 0x14,				// 01F00000h - 01FFFFFFh,1M   Upper 1/32
	W25Q_TOP32	= 0x18,				// 01E00000h - 01FFFFFFh,2M   Upper 1/16
	W25Q_TOP64	= 0x1C,				// 01C00000h - 01FFFFFFh,4M   Upper 1/8
	W25Q_TOP128	= 0x20,				// 01800000h - 01FFFFFFh,8M   Upper 1/4
	W25Q_TOP256	= 0x24				// 01000000h - 01FFFFFFh,16M  Upper 1/2
}lock_area;

struct W25Q256
{
	struct rt_spi_device *flash_device;
	
};
typedef struct W25Q256 *W25Q256_t;

#endif // w25q256fv.h
