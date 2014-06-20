// USART.H
// Definitions file for USART routines
// MAH - 06/2014 - Initial Version

#ifndef __USART_H_
#define __USART_H_

#define USART_BAUD          (115200)
#define USART_WORDLENGTH    (USART_WordLength_8b)
#define USART_STOPBITS      (USART_StopBits_1)
#define USART_PARITY        (USART_Parity_No)
#define USART_FLOWCTRL      (USART_HardwareFlowControl_None)
#define USART_MODE          (USART_Mode_Rx | USART_Mode_Tx)

#define USART_FIFO_SIZE (128)
#define USART_FIFO_MASK (USART_FIFO_SIZE-1)

#if ( USART_FIFO_SIZE & USART_FIFO_MASK )
    #error RX buffer size is not a power of 2
#endif

// Public members

void USART2_Init(void);
void USART2_Send_Byte(uint8_t data);
void USART2_Receive_Byte(uint8_t *data);

uint8_t USART2_Get_Tx_Tail(void);
uint8_t USART2_Get_Tx_Head(void);
void USART2_Increment_Tx_Tail (void);
uint8_t USART2_DataFromTXBuf(uint8_t position);

uint8_t USART2_Get_Rx_Tail(void);
uint8_t USART2_Get_Rx_Head(void);
void USART2_Increment_Rx_Head(void);
void USART2_DataToRXBuf(uint8_t data, uint8_t position) ;

#endif /* __USART_H_ */
