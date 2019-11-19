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
#define btn_enable		0		//����ʹ��
#define auto_mode		1		//�Կ�ģʽ
#define oiltank_start	2		//�ͱ���
#define oiltank_stop	3		//�ͱ�ͣ
#define highcut_start	4		//�ظ���
#define haghcut_stop	5		//�ظ�ͣ
#define lowcut_start	6		//�ص���
#define lowcut_stop		7		//�ص�ͣ
#define twofunc_start	8		//������
#define twofunc_stop	9		//����ͣ
#define fan_start		10		//�����
#define fan_stop		11		//���ͣ
#define pump_start		12		//ˮ����
#define pump_stop		13		//ˮ��ͣ
// button.c action bit
#define cutarm_out		0		//�ظ����
#define cutarm_back		1		//�ظ����
#define star_forward	2		//������ת
#define star_reverse	3		//���ַ�ת
#define onefunc_forward	4		//һ����ת
#define onefunc_reverse	5		//һ�˷�ת
#define twofunc_forward	6		//������ת
#define twofunc_reverse	7		//���˷�ת
#define plate_up		8		//������
#define plate_down		9		//���彵
#define rearprop_up		10		//��֧����
#define rearprop_down	11		//��֧�Ž�


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
