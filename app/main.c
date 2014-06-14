
#include "stm32f2xx.h"
#include "systick.h"
#include "usart.h"
#include "dac.h"

// Main loop. Here's where the action happens!
void main(void)
{
  
    SYSTICK_Init();    // System timer tick initialization
    DAC_HwInit();      // DAC Init
    USART2_Init();     // USART2 Init - Wifi
    
    // Main loop will execute forever
    for(;;)
    {
        // Tests USART2 by sending a byte. Then waits 100ms.
        USART2_Test();
        SYSTICK_delay_10ms(10);
    }
}


