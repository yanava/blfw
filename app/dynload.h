// dynload.h
// Header file for Dynamic Load module

#include <stdint.h>

#ifndef __DYNLOAD_H_
#define __DYNLOAD_H_

void DL_Init(void);
void DL_SetCurrent(uint16_t set_current_in_ma);
uint16_t DL_GetCurrent(void);
uint16_t DL_GetVoltage(void);
void DL_Process(void);
void DL_Timertick(void);
int DL_GetLockedStatus(void);

#endif
