#include "drv_lcd.h"
#include "c_power.h"
#include "cllogo.h"

#define DBG_ENABLE 
#define DBG_SECTION_NAME  "app.init"
#define DBG_LEVEL          DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

/*
��Ҫ�û��Լ���ɵĳ�ʼ������
1��LCD
2, FLASH
3, SD CARD
4��BUTUON
5, rocker(����ad7739)
6��fram
*/

int app_init(void)
{
    int result;
    rt_pin_mode(LCD_PWR_PIN, PIN_MODE_OUTPUT);
    
    rt_pin_write(LCD_PWR_PIN,1);
    result = hw_spi_flash_init();   //flash��ʼ��
    if(result != RT_EOK)
    {
      LOG_E("flash init error");
    }    
    
    result = hw_lcd_init();        //lcd��ʼ��
    if(result != RT_EOK)
    {
      LOG_E("lcd init error");
    }    
    lcd_clear(BLACK);

    /* show C-Power logo */
    lcd_show_image(0, 0, 240,54, gImage_cllog);
    lcd_set_color(BLACK, WHITE);
    
    result = hw_rocker_init();      //rocker��ʼ��(������ʼ��AD7739)
    if(result != RT_EOK)
    {
      LOG_E("rocker init error");
    }

    result = hw_fram_init();        //fram��ʼ��
    if(result != RT_EOK)
    {
      LOG_E("fram init error");
    }
    
    result = hw_button_init();     //button��ʼ��
    if(result != RT_EOK)
    {
      LOG_E("button init error");
    }
    
//    lcd_blk_enable(); //���������ʼ����ɺ����LCD
    community();//rocker �� button��ʼ��֮����
    return result;
}
INIT_APP_EXPORT(app_init);
