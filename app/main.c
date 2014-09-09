#include "stm32f2xx.h"
#include "systick.h"
#include "usart.h"
#include "dac.h"
#include "adc12.h"
#include "dynload.h"
#include "IV.h"
#include <stdio.h>

/* Application Switches for tests */
#define USB_ECHO_VCP_TEST

/******************************************************************************/

/* USB STACK */
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"

 #ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END ;

/******************************************************************************/

// Main function. Here's where the action happens!
void main(void)
{   
    SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );
  
    USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);
    
    SYSTICK_Init();    // System timer tick initialization
       
    DAC_HwInit  ();    // DAC Init, for some weird reason should be before ADC
    ADC12_Init  ();    // ADC Init    
    USART2_Init ();    // USART2 Init - Wifi
    DL_Init     ();    // Dynamic Load Init
    IV_Init     ();    // IV Curve Tracer Initialization  
    
    // Main loop will execute forever
    while(1)
    {
        IV_Process();   // IV curve process
        DL_Process();   // Dynamic Load process 
    }
}


