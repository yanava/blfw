#ifndef __ADC12_H
#define __ADC12_H

#include "stm32f2xx.h"

void ADC12_Init(void);
void ADC12_Tick(void);
void ADC12_HandleData(void);

#endif /* __ADC12_H */