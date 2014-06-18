// USART.H
// Definitions file for USART routines
// MAH - 06/2014 - Initial Version

#ifndef __USART_H_
#define __USART_H_

// Public members
void USART2_Init(void);
uint32_t USART2_Send_Packet(uint32_t data_length, uint16_t *data);

#endif /* __USART_H_ */
