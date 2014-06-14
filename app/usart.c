// USART.H
// Source file for USART routines
// MAH - 06/2014 - Initial Version

// USART2 PINS
// USART2_CTS - GPIO PD3 - Pin 84
// USART2_RST - GPIO PD4 - Pin 85
// USART2_TX - GPIO PD5 - Pin 86
// USART2_RX - GPIO PD6 - Pin 87
// USART2_CK - GPIO PD7 - Pin 88

#include "stm32f2xx_usart.h"
#include "usart.h"

#define USART_BAUD          (115200)
#define USART_WORDLENGTH    (USART_WordLength_8b)
#define USART_STOPBITS      (USART_StopBits_1)
#define USART_PARITY        (USART_Parity_No)
#define USART_FLOWCTRL      (USART_HardwareFlowControl_RTS_CTS)
#define USART_MODE          (USART_Mode_Tx)


// USART2 Init Function
void USART2_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // Enable GPIOB
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    // Connect PB pinst to USART2 Alternate function*/
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
    
    // Configure USART2 GPIO pins as alternate function */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | 
                                  GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    // Enable USART2 clock 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    // USART2 Configuration
    USART_InitStructure.USART_BaudRate            = USART_BAUD;
    USART_InitStructure.USART_WordLength          = USART_WORDLENGTH;
    USART_InitStructure.USART_StopBits            = USART_STOPBITS;
    USART_InitStructure.USART_Parity              = USART_PARITY;
    USART_InitStructure.USART_HardwareFlowControl = USART_FLOWCTRL;
    USART_InitStructure.USART_Mode                = USART_MODE;
    
    // Init USART2 Peripheral
    USART_Init(USART2, &USART_InitStructure);
    
    // Enable USART2
    USART_Cmd(USART2, ENABLE);
}

