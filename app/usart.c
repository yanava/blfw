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

typedef struct USART_BUFFER_TAG
{
    uint8_t tx_buf[USART_FIFO_SIZE];
    uint8_t tx_head;
    uint8_t tx_tail;
    uint8_t rx_buf[USART_FIFO_SIZE];
    uint8_t rx_head;
    uint8_t rx_tail;
} USART_BUFFER_T;

static USART_BUFFER_T usart2_buffer;

// USART2 Init Function
void USART2_Init(void)
{
  
    // Init structures for GPIO, USART and NVIC
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
        
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
    // New FIFO head points to next available data slot
    uint8_t new_head = ((usart2_buffer.tx_head + 1) & USART_FIFO_MASK);
                         
    // Overflow imminence, we should do something                     
    if ( new_head == usart2_buffer.tx_tail)
        return;

    // Free space available, put data on TX FIFO and advance head
    else
    {
        usart2_buffer.tx_buf[new_head] = data;
        usart2_buffer.tx_head = new_head;
        
        // Turns on the TXE interrupt, so data can be sent via interrupt
        USART_ITConfig (USART2, USART_IT_TXE , ENABLE);
        
        return;
    }
}

// Gets one byte received via UART from the buffer
void USART2_Receive_Byte(uint8_t *data)
{
    // Underflow, we should do something
     if(usart2_buffer.tx_head == usart2_buffer.tx_tail)
        return;
    
     // New data has arrived, get the next byte
     else
     {
         uint8_t new_tail = ((usart2_buffer.rx_tail + 1) & USART_FIFO_MASK);   
        
         *data = usart2_buffer.rx_buf[new_tail];
         usart2_buffer.rx_tail = new_tail;      
     } 
}

// Gets TX Tail
uint8_t USART2_Get_Tx_Tail(void)
{
    return usart2_buffer.tx_tail;
}

// Increments TX Tail
void USART2_Increment_Tx_Tail (void)
{
    usart2_buffer.tx_tail++;
}

// Gets TX Head
uint8_t USART2_Get_Tx_Head(void)
{
    return usart2_buffer.tx_head;
}

// Get data from TX Buf
uint8_t USART2_DataFromTXBuf(uint8_t position) 
{
    return usart2_buffer.tx_buf[position];
}

// Gets RX Tail
uint8_t USART2_Get_Rx_Tail(void)
{
    return usart2_buffer.rx_tail;
}

// Gets RX Head
uint8_t USART2_Get_Rx_Head(void)
{
    return usart2_buffer.rx_head;
}

// Increments RX Head
void USART2_Increment_Rx_Head(void)
{
    usart2_buffer.rx_head++;
}

// Put data on RX Buf
void USART2_DataToRXBuf(uint8_t data, uint8_t position) 
{
    usart2_buffer.rx_buf[position] = data;
}

