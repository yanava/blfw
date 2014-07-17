
#include "stm32f2xx.h"
#include "systick.h"
#include "usart.h"
#include "dac.h"
#include "adc12.h"
#include "IV.h"

#include "wifi_bglib.h"

// Main function. Here's where the action happens!
void main(void)
{   
    SYSTICK_Init();    // System timer tick initialization
    DAC_HwInit  ();    // DAC Init
    ADC12_Init  ();    // ADC Init
    USART2_Init ();    // USART2 Init - Wifi
    IV_Init     ();    // IV Curve Tracer Initialization
    
    IV_Test(200);
    
    // Main loop will execute forever
    while(1)
    {
        IV_Process();
    }
    
}


