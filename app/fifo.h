// FIFO.H
// Header file for GENERIC FIFO routines
// MAH - 06/2014 - Initial Version

#ifndef __FIFO_H_
#define __FIFO_H_

#include <string.h>
#include <stdint.h>

// FIFO type
typedef struct FIFO_TAG
{
    uint8_t    *base;
    uint8_t    *head;
    uint8_t    *tail;
    uint8_t     elements;
    size_t      element_size;
} FIFO_T;

void FIFO_Init(FIFO_T *me, void *buffer, uint8_t elements, size_t element_size);
void FIFO_Post(FIFO_T *me, void *element);
void FIFO_Get(FIFO_T *me, void *element);
uint8_t FIFO_AvailableSpace(FIFO_T *me);

#endif // __QUEUE_H_