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
    
    DL_SetCurrent(600);
    
    // Main loop will execute forever
    while(1)
    {
        //IV_Process();   // IV curve process
        DL_Process();
    }
    
}


