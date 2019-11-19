#ifndef __C_POWER_H__
#define __C_POWER_H__
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <time.h>
#include <stdio.h>
#include <dfs_fs.h>
#include <dfs_posix.h>
#include <drv_ad7739.h>
#include <drv_spi.h>
#include <drv_fram.h>
#include "string.h"
#include "spi_flash.h"
#include "spi_flash_sfud.h"
#include "drv_lcd.h"

/*********  project version  **********/

#define CL_VERSION                      1L              /**< major version number */
#define CL_SUBVERSION                   0L              /**< minor version number */
#define CL_REVISION                     0L              /**< revise version number */


#define LED_RUN_PIN     GET_PIN(C, 6)
#define LED_ERR_PIN     GET_PIN(C, 7)
#define LCD_PWR_PIN     GET_PIN(D, 13)


/**************  rocker  ***************/

#define ROCKER_THREAD_NAME	"rocker"
#define ROCKER_THREAD_PRIORITY 12
#define ROCKER_THREAD_STACK_SIZE 1024
// 使能按钮
#define Clip_Hand       GET_PIN(C, 5)// DI14
#define Left_Wheel      GET_PIN(C, 4)// DI15 左摇
#define Right_Wheel     GET_PIN(B, 1)// DI16 右摇


/**************  button  ****************/

#define BUTTON_THREAD_NAME	"button"
#define BUTTON_THREAD_PRIORITY 11
#define BUTTON_THREAD_STACK_SIZE 512
// button.c steady bit
#define btn_enable		0		//控制使能
#define auto_mode		1		//自控模式
#define oiltank_start	2		//油泵启
#define oiltank_stop	3		//油泵停
#define highcut_start	4		//截高启
#define haghcut_stop	5		//截高停
#define lowcut_start	6		//截低启
#define lowcut_stop		7		//截低停
#define twofunc_start	8		//二运启
#define twofunc_stop	9		//二运停
#define fan_start		10		//风机启
#define fan_stop		11		//风机停
#define pump_start		12		//水泵启
#define pump_stop		13		//水泵停
// button.c action bit
#define cutarm_out		0		//截割臂伸
#define cutarm_back		1		//截割臂缩
#define star_forward	2		//星轮正转
#define star_reverse	3		//星轮反转
#define onefunc_forward	4		//一运正转
#define onefunc_reverse	5		//一运反转
#define twofunc_forward	6		//二运正转
#define twofunc_reverse	7		//二运反转
#define plate_up		8		//铲板升
#define plate_down		9		//铲板降
#define rearprop_up		10		//后支撑升
#define rearprop_down	11		//后支撑降


void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void lcd_blk_enable(void);
    
int hw_lcd_init(void);
int hw_fram_init(void);
int hw_rocker_init(void);
int hw_button_init(void);
int hw_spi_flash_init(void);
rt_uint32_t rocker_get(void);//获取 rocker 数据
rt_uint32_t button_get(void);//获取 button 数据
int community(void);//初始化打印任务
#endif
