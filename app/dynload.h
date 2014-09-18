// dynload.h
// Header file for Dynamic Load module

#include <stdint.h>

#ifndef __DYNLOAD_H_
#define __DYNLOAD_H_

#define DL_MAX_CURRENT              (8000)
#define DL_MIN_CURRENT              (100)

// Return values
enum DL_RETURN_VAL
{
    DL_NOERROR     = 0,
    DL_OFFSCALE    = -1
};

int DL_SetCurrent(uint16_t set_current_in_ma);
uint16_t DL_GetCurrent(void);
uint16_t DL_GetVoltage(void);

#endif
