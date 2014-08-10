// dynload.h
// Header file for Dynamic Load module

#include <stdint.h>

#ifndef __DYNLOAD_H_
#define __DYNLOAD_H_

#define DL_CURRENT_RESISTOR         (0.1f)
#define DL_PID_KP                   (1.5f)
#define DL_PID_KI                   (0.5f)
#define DL_PID_KD                   (0.3f)
#define DL_ADC_ISENSE_LB1           (ADC12_CH2)

void DL_Init(void);
void DL_SetCurrent(uint16_t set_current_in_ma);
void DL_Process(void);

#endif
