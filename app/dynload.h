// dynload.h
// Header file for Dynamic Load module

#include <stdint.h>

#ifndef __DYNLOAD_H_
#define __DYNLOAD_H_

#define DL_CURRENT_RESISTOR         (0.1f)
#define DL_VOLTAGE_DIVIDER          (33.4f)
#define DL_PID_KP                   (0.35f)
#define DL_PID_KI                   (0.05f)
#define DL_PID_KD                   (0.04f)
#define DL_ADC_VSENSE               (ADC12_CH1)
#define DL_ADC_ISENSE_LB1           (ADC12_CH2)

void DL_Init(void);
void DL_SetCurrent(uint16_t set_current_in_ma);
uint16_t DL_GetCurrent(void);
uint16_t DL_GetVoltage(void);
void DL_Process(void);
void DL_Timertick(void);
int DL_GetLockedStatus(void);

#endif
