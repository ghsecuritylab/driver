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
// ʹ�ܰ�ť
#define Clip_Hand       GET_PIN(C, 5)// DI14
#define Left_Wheel      GET_PIN(C, 4)// DI15 ��ҡ
#define Right_Wheel     GET_PIN(B, 1)// DI16 ��ҡ


/**************  button  ****************/

#define BUTTON_THREAD_NAME	"button"
#define BUTTON_THREAD_PRIORITY 11
#define BUTTON_THREAD_STACK_SIZE 512
// button.c steady bit
#define OPC_BTN_ENBLE       0       //����ʹ��
#define OPC_AUTO_MODE       1       //�Կ�ģʽ
#define OPC_OILTANK_START   2       //�ͱ���
#define OPC_OILTANK_STOP    3       //�ͱ�ͣ
#define OPC_HIGHCUT_START   4       //�ظ���
#define OPC_HIGHCUT_STOP    5       //�ظ�ͣ
#define OPC_LOWCUT_START    6       //�ص���
#define OPC_LOWCUT_STOP     7       //�ص�ͣ
#define OPC_TWOFUNC_START   8       //������
#define OPC_TWOFUNC_STOP    9       //����ͣ
#define OPC_FAN_START       10      //�����
#define OPC_FAN_STOP        11      //���ͣ
#define OPC_PUMP_START      12      //ˮ����
#define OPC_PUMP_STOP       13      //ˮ��ͣ
#define OPC_STUDY_MODE		14		//ѧϰģʽ
#define OPC_SYSTEM_SHARP	15		//ϵͳ��ͣ
// button.c action bit
#define OPC_CUTARM_OUT      0       //�ظ����
#define OPC_CUTARM_BACK     1       //�ظ����
#define OPC_STAR_FORWARD    2       //������ת
#define OPC_STAR_REVERSE    3       //���ַ�ת
#define OPC_ONEFUNC_FORWARD 4       //һ����ת
#define OPC_ONRFUNC_REVERSE 5       //һ�˷�ת
#define OPC_RADAR_OUT		6       //�״���
#define OPC_RADAR_BACK		7       //�״���
#define OPC_PLATE_UP        8       //������
#define OPC_PLATE_DOWN      9       //���彵
#define OPC_REARPROP_UP     10      //��֧����
#define OPC_REARPROP_DOWN   11      //��֧�Ž�
#define OPC_BELL_RING		12		//����


void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void lcd_blk_enable(void);
    
int hw_lcd_init(void);
int hw_fram_init(void);
int hw_rocker_init(void);
int hw_button_init(void);
int hw_spi_flash_init(void);
rt_uint32_t rocker_get(void);//��ȡ rocker ����
rt_uint32_t button_get(void);//��ȡ button ����
int community(void);//��ʼ����ӡ����
#endif
