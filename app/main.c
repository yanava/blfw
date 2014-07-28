
#include "stm32f2xx.h"
#include "systick.h"
#include "usart.h"
#include "dac.h"
#include "adc12.h"
#include "IV.h"
#include "wifi_bglib.h"

#include <stdio.h>

// Main function. Here's where the action happens!
void main(void)
{   
    SYSTICK_Init();    // System timer tick initialization
    ADC12_Init  ();    // ADC Init
    DAC_HwInit  ();    // DAC Init

    USART2_Init ();    // USART2 Init - Wifi
    IV_Init     ();    // IV Curve Tracer Initialization
    
    IV_Perform_Curve();
    
    // Main loop will execute forever
    while(1)
    {
        IV_Process();    
    }
    
}


