#ifndef __AD7739_H__
#define __AD7739_H__

#include <board.h>


#define USING_AD7739_DEFAULT_CONFIG

/**********default config***********/
const static rt_uint8_t AD7739_Config[4] =
{
	0x38,	//IO_PORT_VALUE			
	0x0D,	//CHANNEL_SET_VALUE		参考电压：2.5V，通道连续转换
	0x91,	//CHANNEL_CONV_TIME
	0x22,	//MODE_SET_VALUE		连续转换模式，24位模式
};		


struct AD7739
{
	struct rt_spi_device *spi_device;
	
	char *adc_device_name;
	
	rt_uint8_t channel_enable;
	rt_uint8_t reset_pin;
	rt_uint8_t ready_pin;
};
typedef struct AD7739 *AD7739_t;


/**********register addr************/
#define AD7739_WRITE			0x00
#define	AD7739_READ				0x40

#define	AD7739_COMM	        	0x00
#define	AD7739_IOPORT			0x01
#define AD7739_REV				0x02
#define AD7739_TEST				0x03
#define	AD7739_ADC_STATUS		0x04
#define	AD7739_CHECKSUM			0x05
#define ADC7739_ADC_ZERO_SCALE  0x06
#define ADC7739_ADC_FULL_SCALE  0x07
#define ADC7739_CHAN_DATA       0x08
#define	AD7739_CHAN_ZERO_SCALE	0x10
#define	AD7739_CHAN_FULL_SCALE	0x18
#define	AD7739_CHAN_STATUS		0x20
#define	AD7739_CHAN_SETUP       0x28
#define AD7739_CHAN_CONV_TIME	0x30
#define AD7739_MODE	        	0x38


rt_size_t ad7739_write(AD7739_t device,rt_uint8_t addr,const void *value);
rt_size_t ad7739_read(AD7739_t device,rt_uint8_t addr,void *buffer,rt_uint8_t length);
void ad7739_channel_read(AD7739_t device,rt_uint8_t *buffer,rt_uint8_t length);

rt_err_t ad7739_init(const char *spi_bus_name,AD7739_t adc,
				GPIO_TypeDef *cs_gpiox, uint16_t cs_gpio_pin);

void ad7739_deinit(AD7739_t device);



#endif //ad7739.h
