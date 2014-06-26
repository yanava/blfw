// USART.C
// Source file for USART routines
// MAH - 06/2014 - Initial Version

// USART2 PINS
// USART2_CTS - GPIO PD3 - Pin 84
// USART2_RST - GPIO PD4 - Pin 85
// USART2_TX - GPIO PD5 - Pin 86
// USART2_RX - GPIO PD6 - Pin 87
// USART2_CK - GPIO PD7 - Pin 88

#include "stm32f2xx_usart.h"
#include "fifo.h"
#include "usart.h"

typedef struct USART_FIFO_TAG
{
    FIFO_T   super; 
    uint8_t  buf[USART_FIFO_SIZE];
} USART_FIFO_T;

static USART_FIFO_T usart2_tx_fifo;
static USART_FIFO_T usart2_rx_fifo;

// USART2 Init Function
void USART2_Init(void)
{
    // Init structures for GPIO, USART and NVIC
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // Initialize FIFOs
    FIFO_Init((FIFO_T*) &usart2_tx_fifo,
               &usart2_tx_fifo.buf,
               USART_FIFO_SIZE,
               sizeof(usart2_tx_fifo.buf[0]) / sizeof(uint8_t));
    FIFO_Init((FIFO_T*) &usart2_rx_fifo,
               &usart2_rx_fifo.buf,
               USART_FIFO_SIZE,
               sizeof(usart2_rx_fifo.buf[0]) / sizeof(uint8_t));
        
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
    
    // Enable USART2 IRQ channel in the NVIC controller.
    // Interrupts are triggered upon data reception
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
        
    // Configure USART2 Interrupt for data reception triggering
    USART_ITConfig (USART2, USART_IT_RXNE , ENABLE);
       
    // Enable USART2
    USART_Cmd(USART2, ENABLE);  
}

// Sends one byte via the UART
void USART2_Send_Byte(uint8_t data)
{
    // Puts the data on the FIFO
    FIFO_Post(&usart2_tx_fifo.super, &data);    
    
    // Turns on the TX Empty interrupt for UART2
    USART_ITConfig (USART2, USART_IT_TXE , ENABLE);
}

// Sends multiple bytes of data over the UART2
// If there is no available space on the FIFO, returns without sending the data

void USART2_Send_Packet(uint32_t length, uint8_t *data)
{    
    // You are trying to send more bytes than FIFO has space for.
    if (length > FIFO_AvailableElements(&usart2_tx_fifo.super))
        return;
   
    // FIFO has available space for your data
    else 
    { 
        // Put all your bytes on the FIFO
        for(int i = 0; i < length; i++)
            USART2_Send_Byte(data[i]);
    }
}

// Gets one byte received via UART from the buffer
void USART2_Receive_Byte(uint8_t *data)
{
    // Gets a byte from the FIFO
    FIFO_Get(&usart2_tx_fifo.super,data); 
}

// Gets one byte received via UART from the buffer
void USART2_Receive_Packet(uint8_t *data,  uint8_t length)
{
    // You are trying to get more bytes from the FIFO than there are new
    // received bytes. 
    if (length > FIFO_AvailableElements(&usart2_rx_fifo.super))
        return;
    // You are trying to get an OK amount of bytes
    else 
    { 
        // Retrieve all your bytes from the FIFO
        for(int i = 0; i < length; i++)
            USART2_Receive_Byte(&data[i]);
    }
}

// Gets the USART TX FIFO as a pure FIFO structure
FIFO_T* USART2_GetTXFifoT(void)
{
    return (&usart2_tx_fifo.super);
}

// Gets the USART RX FIFO as a pure FIFO structure
FIFO_T* USART2_GetRXFifoT(void)
{
    return (&usart2_rx_fifo.super);
}