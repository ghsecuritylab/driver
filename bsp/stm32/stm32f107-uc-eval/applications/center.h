#ifndef __CENTER_H__
#define __CENTER_H__

#include <board.h>

/***define sensor struct***/
typedef struct ad_sensor
{
	float sensor_value;
	float radio;
	float offset;
	float full;
	float zero;
}ad_sensor;


/*******use for fm25cl64 ops*******/
static rt_uint8_t fm_protect = 0x08;	// protect the upper 1/4 memory area ,from 0x1800 to 0x1fff
static rt_uint8_t fm_unprotect = 0x00;

/*
* define the address of fm25cl64 to store the channel data
* each channel have 4 data , each data take 4 address
*/

#define CHANNEL0_RADIO		0x1800
#define CHANNEL0_OFFSET		0x1804
#define CHANNEL0_FULL		0x1808
#define CHANNEL0_ZERO		0x180c

#define	CHANNEL1_RADIO		0x1810
#define	CHANNEL1_OFFSET		0x1814
#define	CHANNEL1_FULL		0x1818
#define	CHANNEL1_ZERO		0x181c

#define	CHANNEL2_RADIO		0x1820
#define	CHANNEL2_OFFSET		0x1824
#define	CHANNEL2_FULL		0x1828
#define	CHANNEL2_ZERO		0x182c

#define	CHANNEL3_RADIO		0x1830
#define	CHANNEL3_OFFSET		0x1834
#define	CHANNEL3_FULL		0x1838
#define	CHANNEL3_ZERO		0x183c

#define	CHANNEL4_RADIO		0x1840
#define	CHANNEL4_OFFSET		0x1844
#define	CHANNEL4_FULL		0x1848
#define	CHANNEL4_ZERO		0x184c

#define	CHANNEL5_RADIO		0x1850
#define	CHANNEL5_OFFSET		0x1854
#define	CHANNEL5_FULL		0x1858
#define	CHANNEL5_ZERO		0x185c

#define	CHANNEL6_RADIO		0x1860
#define	CHANNEL6_OFFSET		0x1864
#define	CHANNEL6_FULL		0x1868
#define	CHANNEL6_ZERO		0x186c

#define	CHANNEL7_RADIO		0x1870
#define	CHANNEL7_OFFSET		0x1874
#define	CHANNEL7_FULL		0x1878
#define	CHANNEL7_ZERO		0x187c

#define	CHANNEL8_RADIO		0x1880
#define	CHANNEL8_OFFSET		0x1884
#define	CHANNEL8_FULL		0x1888
#define	CHANNEL8_ZERO		0x188c

#define	CHANNEL9_RADIO		0x1890
#define	CHANNEL9_OFFSET		0x1894
#define	CHANNEL9_FULL		0x1898
#define	CHANNEL9_ZERO		0x189c

#define	CHANNEL10_RADIO		0x18a0
#define	CHANNEL10_OFFSET	0x18a4
#define	CHANNEL10_FULL		0x18a8
#define	CHANNEL10_ZERO		0x18ac

#define	CHANNEL11_RADIO		0x18b0
#define	CHANNEL11_OFFSET	0x18b4
#define	CHANNEL11_FULL		0x18b8
#define	CHANNEL11_ZERO		0x18bc

#define	CRC_VALUE			0x18c0



#endif //center.h
