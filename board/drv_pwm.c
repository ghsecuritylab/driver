#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>
#include <stdio.h>
#include "c_power.h"

TIM_HandleTypeDef      TIM4_Handler;
TIM_OC_InitTypeDef     TIM4_CH2Handler;
TIM_ClockConfigTypeDef sClockSourceConfig;

static int rt_hw_pwm_init(void)
{
    TIM4_Handler.Instance = TIM4;
    TIM4_Handler.Init.Prescaler = 200-1;
    TIM4_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM4_Handler.Init.Period = 500-1;
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    TIM4_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&TIM4_Handler);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&TIM4_Handler, &sClockSourceConfig);
    HAL_TIM_PWM_Init(&TIM4_Handler);
    
    return RT_EOK;
}
//INIT_ENV_EXPORT(rt_hw_pwm_init);

void lcd_blk_enable(void)
{
    rt_hw_pwm_init();
        
    TIM4_CH2Handler.OCMode=TIM_OCMODE_PWM1;
    TIM4_CH2Handler.Pulse = 250; 
    TIM4_CH2Handler.OCPolarity=TIM_OCPOLARITY_LOW; 
    HAL_TIM_PWM_ConfigChannel(&TIM4_Handler,&TIM4_CH2Handler,TIM_CHANNEL_2);
    HAL_TIM_MspPostInit(&TIM4_Handler);
    HAL_TIM_PWM_Start(&TIM4_Handler,TIM_CHANNEL_2);
}

void setblk(int argc,char** argv)
{
    
    if (!strcmp(argv[1],"0"))
    {
      TIM4_CH2Handler.Pulse = 450; 
    }

    if (!strcmp(argv[1],"1"))
    {
      TIM4_CH2Handler.Pulse = 400; 
    }

    if (!strcmp(argv[1],"2"))
    {
      TIM4_CH2Handler.Pulse = 300; 
    }

    if (!strcmp(argv[1],"3"))
    {
      TIM4_CH2Handler.Pulse = 200; 
    }
    
    if (!strcmp(argv[1],"4"))
    {
      TIM4_CH2Handler.Pulse = 100; 
    }
    
    if (!strcmp(argv[1],"5"))
    {
      TIM4_CH2Handler.Pulse = 10; 
    }
    HAL_TIM_PWM_ConfigChannel(&TIM4_Handler,&TIM4_CH2Handler,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&TIM4_Handler,TIM_CHANNEL_2);
}
MSH_CMD_EXPORT(setblk, set blk);
