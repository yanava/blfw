#include "stm32f2xx.h"

#include "systick.h"
#include "dac.h"


int main()
{
  
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  SYSTICK_Init();
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  // Init GPIOE Outputs
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
    
  // Init GPIOE Inputs
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  DAC_HwInit();
    
  for(;;)
  {
    if (!GPIO_ReadInputDataBit (GPIOE, GPIO_Pin_5)) GPIO_SetBits(GPIOE, GPIO_Pin_1);
    else GPIO_ResetBits(GPIOE, GPIO_Pin_1);
    
    if (!GPIO_ReadInputDataBit (GPIOE, GPIO_Pin_6)) GPIO_SetBits(GPIOE, GPIO_Pin_2);
    else GPIO_ResetBits(GPIOE, GPIO_Pin_2);
    
    SYSTICK_delay_10ms(10);
  }
}


