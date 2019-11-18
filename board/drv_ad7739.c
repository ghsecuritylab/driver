#include "c_power.h"

#ifdef BSP_USING_Board_AD
#define DBG_ENABLE
#define DBG_SECTION_NAME  "drv.ad7739"
#define DBG_LEVEL          DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

/*
* 函数名称：ad7739_reset
* 函数功能：复位AD7739设备
*/
void ad7739_reset(AD7739_t device)
{
    rt_pin_write(device->reset_pin,PIN_LOW);
    rt_thread_delay(5);
    rt_pin_write(device->reset_pin,PIN_HIGH);
}

/*
* 函数名称：ad7739_write
* 函数功能：向指定寄存器写入参数
* 输入参数：addr 寄存器地址；value 拟写入的参数地址（单字节数据）
*/
rt_size_t ad7739_write(AD7739_t device,rt_uint8_t addr,const void *value)
{
    static rt_uint8_t cmd;
    cmd = AD7739_WRITE|addr;

    rt_spi_send(device->spi_device,&cmd,1);
    return rt_spi_send(device->spi_device,value,1);
}

/*
* 函数名称：ad7739_read
* 函数功能：从指定寄存器读取参数
* 输入参数：addr 寄存器地址；buffer 存储数据的地址；length 拟读取数据字节长度
*/
rt_size_t ad7739_read(AD7739_t device,rt_uint8_t addr,void *buffer,rt_uint8_t length)
{
    static rt_uint8_t cmd;
    cmd = AD7739_READ|addr;

    rt_spi_send(device->spi_device,&cmd,1);
    return rt_spi_recv(device->spi_device,buffer,length);
}

/*
* 函数名称：ad7739_channel_read
* 函数功能：读取通道转换后的数据
* 输入参数：buffer 存储数据的地址；length 单通道数据字节数（取决与AD模式 24bit or 16bit）
*/
void ad7739_channel_read(AD7739_t device,rt_uint8_t *buffer,rt_uint8_t length)
{
    rt_uint8_t adc_status = 0x00;

    /*** waitting for all enabled channel translate over ***/
    while(adc_status!=device->channel_enable)
    {
        ad7739_read(device,AD7739_ADC_STATUS,&adc_status,1);
        rt_thread_delay(1);
    }

    for(int i=0; i<8; i++)
    {
        if((device->channel_enable>>i)&0x01)
        {
            ad7739_read(device,ADC7739_CHAN_DATA+i,buffer,length);
        }
        buffer += length;
    }
}


static void ad7739_config(AD7739_t device,rt_uint8_t chan_enable)
{
    ad7739_write(device,AD7739_IOPORT,&AD7739_Config[0]);

    for(int i=0; i<8; i++)
    {
        if((chan_enable>>i)&0x01)
        {
            ad7739_write(device,AD7739_CHAN_SETUP+i,&AD7739_Config[1]);
            ad7739_write(device,AD7739_CHAN_CONV_TIME+i,&AD7739_Config[2]);
            ad7739_write(device,AD7739_MODE+i,&AD7739_Config[3]);
        }
    }
}


rt_err_t ad7739_init(const char *spi_bus_name,AD7739_t device,
                     GPIO_TypeDef *cs_gpiox, uint16_t cs_gpio_pin)
{
    rt_hw_spi_device_attach(spi_bus_name,device->adc_device_name,cs_gpiox,cs_gpio_pin);

    rt_pin_mode(device->reset_pin,PIN_MODE_OUTPUT);
    rt_pin_mode(device->ready_pin,PIN_MODE_INPUT_PULLUP);

    device->spi_device = (struct rt_spi_device *)rt_device_find(device->adc_device_name);

    if(device->spi_device==RT_NULL)
    {
        LOG_E("ADC device find error! Please check!\n");
        return -RT_ERROR;
    }

    /*        SPI Config        */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
        cfg.max_hz = 6 * 1000 *1000;	//6MHz
        rt_spi_configure(device->spi_device,&cfg);
    }

    /*          Reset AD7739        */
    ad7739_reset(device);

#ifdef  USING_AD7739_DEFAULT_CONFIG
    ad7739_config(device,device->channel_enable);
#endif

    LOG_I("%s init success!",device->adc_device_name);
    return RT_EOK;
}

void ad7739_deinit(AD7739_t device)
{
    RT_ASSERT(device!=RT_NULL);
    rt_free(device);
}

#endif
