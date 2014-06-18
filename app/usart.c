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
#define USART_FLOWCTRL      (USART_HardwareFlowControl_None)
#define USART_MODE          (USART_Mode_Rx | USART_Mode_Tx)

#define USART2_FIFO_SIZE 128

uint8_t USART2_TxFifo[USART2_FIFO_SIZE];
uint8_t USART2_RxFifo[USART2_FIFO_SIZE];


// USART2 Init Function
void USART2_Init(void)
{
  
    // Init structures for GPIO, USART and NVIC
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;
        
    // Enable AHB clock to GPIOD
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    // Connect PB pins to USART2 by setting to Alternate Function*/
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
    
    // Configure USART2 GPIO pins as Alternate Function */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | 
                                  GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    // Enable APB clock to USART2 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    // USART2 Configuration
    // Default is 115200 8n1 no flow control
    USART_InitStructure.USART_BaudRate            = USART_BAUD;
    USART_InitStructure.USART_WordLength          = USART_WORDLENGTH;
    USART_InitStructure.USART_StopBits            = USART_STOPBITS;
    USART_InitStructure.USART_Parity              = USART_PARITY;
    USART_InitStructure.USART_HardwareFlowControl = USART_FLOWCTRL;
    USART_InitStructure.USART_Mode                = USART_MODE;
    
    // Init USART2 Peripheral
    USART_Init(USART2, &USART_InitStructure);
    
    /* At the moment USART via Interrupt is disabled!!
    
    // Enable USART2 IRQ channel in the NVIC controller.
    // Interrupts are triggered upon data reception
    //NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);
        
    // Configure USART2 Interrupt for data reception triggering
    //USART_ITConfig (USART2, USART_IT_RXNE, ENABLE);
    
    */
    
    // Enable USART2
    USART_Cmd(USART2, ENABLE);
       
}


// This is blocking code. It's as bad as having sex without condoms.
// Replace this as soon as you get the chance
void USART2_Send_Byte(uint16_t data)
{
    // Sends the data info
    USART_SendData (USART2, data);
    
    // Waits until the data is gone
    while (USART_GetFlagStatus(USART2,USART_FLAG_TXE) != SET);
    return;
}

// Sends a packet via UART2
uint32_t USART2_Send_Packet(uint32_t data_length, uint16_t *data)
{
    // Sends bytes until theres nothing more to be sent
    for (int i = 0; i < data_length; i++)
        USART2_Send_Byte(data[i]);                
    
    // this is pointless but, for now, necessary
    return data_length;
}
          