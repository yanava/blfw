// pid.h
// Header file for PID control module

#include <stdint.h>

#ifndef __PID_H_
#define __PID_H_

#define PID_ERROR_TOL       (1)
#define PID_MAX_INTEGRAL    (65536)

// PID Tyoe
typedef struct PID_TAG
{    
    float Kp;           // Proportional coefficient
    float Ki;           // Integral coefficient
    float Kd;           // Derivative coefficient
    int32_t integral;   // Integral value
    int32_t last_error; // Last error value
    int32_t ref_value;  // Reference value
} PID_T;

// Function prototypes
void PID_Init(PID_T *me, float Kp, float Ki, float Kd);
void PID_SetRefValue(PID_T *me, int32_t ref_value);
int32_t PID_Process(PID_T *me, int32_t measured_value);

#endif