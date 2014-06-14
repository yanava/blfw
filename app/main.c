
#include "stm32f2xx.h"
#include "systick.h"
#include "usart.h"
#include "dac.h"

// Main function. Here's where the action happens!
void main(void)
{
  
    SYSTICK_Init();    // System timer tick initialization
    DAC_HwInit();      // DAC Init
    USART2_Init();     // USART2 Init - Wifi
    
    // Main loop will execute forever
    for(;;)
    {
    }
}


