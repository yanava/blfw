// USART.H
// Source file for USART routines
// MAH - 06/2014 - Initial Version

// USART2 PINS
// USART2_CTS - GPIO PD3 - Pin 84
// USART2_RST - GPIO PD4 - Pin 85
// USART2_TX - GPIO PD5 - Pin 86
// USART2_RX - GPIO PD6 - Pin 87
// USART2_CK - GPIO PD7 - Pin 88


#include "usart.h"
#include "stm32f2xx_usart.h"

void USART2_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // Enable GPIOD
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    // Connect PD pinst to USART2 Alternate function*/
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
    
    
}