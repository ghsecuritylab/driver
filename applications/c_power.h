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
#include "spi_flash.h"
#include "spi_flash_sfud.h"
#include "comm.h"

#define LED_RUN_PIN     GET_PIN(C, 6)
#define LED_ERR_PIN     GET_PIN(C, 7)
#define LCD_PWR_PIN     GET_PIN(D, 13)

// 使能按钮
#define Clip_Hand       GET_PIN(B,10)//GET_PIN(C, 5)// DI14
#define Left_Wheel      GET_PIN(C, 4)// DI15 左摇
#define Right_Wheel     GET_PIN(B, 1)// DI16 右摇

#define ch_amount    4 //ad7739 读取通道数

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void lcd_blk_enable(void);
    
int hw_lcd_init(void);
int hw_fram_init(void);
int hw_rocker_init(void);
int hw_button_init(void);
int hw_spi_flash_init(void);
#endif
