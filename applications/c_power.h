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
#define OPC_BTN_ENBLE       0       //控制使能
#define OPC_AUTO_MODE       1       //自控模式
#define OPC_OILTANK_START   2       //油泵启
#define OPC_OILTANK_STOP    3       //油泵停
#define OPC_HIGHCUT_START   4       //截高启
#define OPC_HIGHCUT_STOP    5       //截高停
#define OPC_LOWCUT_START    6       //截低启
#define OPC_LOWCUT_STOP     7       //截低停
#define OPC_TWOFUNC_START   8       //二运启
#define OPC_TWOFUNC_STOP    9       //二运停
#define OPC_FAN_START       10      //风机启
#define OPC_FAN_STOP        11      //风机停
#define OPC_PUMP_START      12      //水泵启
#define OPC_PUMP_STOP       13      //水泵停
#define OPC_STUDY_MODE		14		//学习模式
#define OPC_SYSTEM_SHARP	15		//系统急停
// button.c action bit
#define OPC_CUTARM_OUT      0       //截割臂伸
#define OPC_CUTARM_BACK     1       //截割臂缩
#define OPC_STAR_FORWARD    2       //星轮正转
#define OPC_STAR_REVERSE    3       //星轮反转
#define OPC_ONEFUNC_FORWARD 4       //一运正转
#define OPC_ONRFUNC_REVERSE 5       //一运反转
#define OPC_RADAR_OUT		6       //雷达伸
#define OPC_RADAR_BACK		7       //雷达缩
#define OPC_PLATE_UP        8       //铲板升
#define OPC_PLATE_DOWN      9       //铲板降
#define OPC_REARPROP_UP     10      //后支撑升
#define OPC_REARPROP_DOWN   11      //后支撑降
#define OPC_BELL_RING		12		//警铃


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
