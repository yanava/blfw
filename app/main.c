
#include "stm32f2xx.h"
#include "systick.h"
#include "usart.h"
#include "dac.h"
#include "fsm.h"
#include "fifo.h"


FIFO_T testQueue; 

typedef struct teststruct_tag
{
    uint8_t member_one;
    uint16_t member_two;
    uint32_t member_three;
} teststruct_t; 

teststruct_t teste[4]; 

// Main function. Here's where the action happens!
void main(void)
{
    
    uint8_t space = 0; 
    
    FIFO_Ctor(&testQueue,&teste,4,sizeof(teste[0])/sizeof(uint8_t));
    
    space = FIFO_AvailableSpace(&testQueue);
    
    teststruct_t element = {1,2,3};
    teststruct_t sample;
    
    FIFO_Post(&testQueue,&element);
    
    space = FIFO_AvailableSpace(&testQueue);
    
    FIFO_Post(&testQueue,&element);
    FIFO_Post(&testQueue,&element);

    space = FIFO_AvailableSpace(&testQueue);
    
    FIFO_Get(&testQueue,&sample);
    
    space = FIFO_AvailableSpace(&testQueue);
      
    FIFO_Post(&testQueue,&element);
    
    space = FIFO_AvailableSpace(&testQueue);
    
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    SYSTICK_Init();    // System timer tick initialization
    DAC_HwInit();      // DAC Init
    USART2_Init();     // USART2 Init - Wifi

     // Enable AHB clock to GPIOD
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    // Configure USART2 GPIO pins as Alternate Function */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_ResetBits(GPIOD, GPIO_Pin_7);
    SYSTICK_delay_ms(10);
    GPIO_SetBits(GPIOD, GPIO_Pin_7);
    
    // Main loop will execute forever
    for(;;)
    {
        
    }
    
}


