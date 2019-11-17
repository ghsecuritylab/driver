#include "drv_lcd.h"
#include "c_power.h"
#include "cllogo.h"

#define DBG_ENABLE 
#define DBG_SECTION_NAME  "app.init"
#define DBG_LEVEL          DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

/*
需要用户自己完成的初始化部分
1，LCD
2, FLASH
3, SD CARD
4，BUTUON
5, rocker
6，fram
*/
#define LCD_PWR_PIN    GET_PIN(D, 13)

int app_init(void)
{
    int result;
    rt_pin_mode(LCD_PWR_PIN,PIN_MODE_OUTPUT);
    
    result = hw_spi_flash_init();   //flash初始化
    if(result != RT_EOK)
    {
      LOG_E("flash init error");
      return -RT_ERROR;
    }    
    rt_pin_write(LCD_PWR_PIN,1);
    result = hw_lcd_init();        //lcd初始化
    if(result != RT_EOK)
    {
      LOG_E("lcd init error");
      return -RT_ERROR;
    }    
    lcd_clear(BLACK);

    /* show C-Power logo */
    lcd_show_image(0, 0, 240,54, gImage_cllog);
    
    result = hw_rocker_init();      //rocker初始化(包含初始了AD7739)
    if(result != RT_EOK)
    {
      LOG_E("rocker init error");
      return -RT_ERROR;
    }

    result = hw_fram_init();        //fram初始化
    if(result != RT_EOK)
    {
      LOG_E("fram init error");
      return -RT_ERROR;
    }
    
    result = hw_button_init();     //button初始化
    if(result != RT_EOK)
    {
      LOG_E("button init error");
      return -RT_ERROR;
    }
    
    return result;
}
INIT_APP_EXPORT(app_init);
