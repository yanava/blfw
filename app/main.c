#include "stm32f2xx.h"
#include "systick.h"
#include "usart.h"
#include "dac.h"
#include "adc12.h"
#include "dynload.h"
//#include "IV.h"

#include <stdio.h>

// Main function. Here's where the action happens!
void main(void)
{   
    SYSTICK_Init();    // System timer tick initialization
    DAC_HwInit  ();    // DAC Init, for some weird reason should be before ADC
    ADC12_Init  ();    // ADC Init    
    USART2_Init ();    // USART2 Init - Wifi
    DL_Init     ();
    //IV_Init     ();    // IV Curve Tracer Initialization
    
    //IV_Perform_Curve();
    
    // DL_SetCurrent(100);
    
    GPIO_InitTypeDef GPIO_InitStructure; 
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    
    // Configure buttons as input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    // Init Inputs
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    // Configure LEDs as Outputs
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    
    // Init Outputs
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    uint16_t current_in_ma = 0;
    uint16_t delay = 150;    
    
    // Main loop will execute forever
    while(1)
    {
        //IV_Process();   // IV curve process
        
        if (!GPIO_ReadInputDataBit (GPIOE, GPIO_Pin_5))
        {
            current_in_ma += 50;
            SYSTICK_delay_ms(delay);
        }
        else if (!GPIO_ReadInputDataBit (GPIOE, GPIO_Pin_6))
        {
            current_in_ma -= 50;
            SYSTICK_delay_ms(delay);
        }   
        
        if (current_in_ma > 33000) 
            current_in_ma = 33000;
        else if (current_in_ma < 200) 
            current_in_ma = 200;
        
        DL_SetCurrent(current_in_ma);
        DL_Process();
        
        if(DL_GetLockedStatus()) GPIO_SetBits (GPIOE, GPIO_Pin_1);
        else GPIO_ResetBits (GPIOE, GPIO_Pin_1);
       
        
    }
    
}


