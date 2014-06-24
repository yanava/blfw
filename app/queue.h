// QUEUE.H
// Header file for GENERIC QUEUE routines
// MAH - 06/2014 - Initial Version

#ifndef __QUEUE_H_
#define __QUEUE_H_

#include <string.h>
#include <stdint.h>

typedef struct QUEUE_TAG
{
    uint8_t    *base;
    uint8_t    *head;
    uint8_t    *tail;
    uint8_t     elements;
    size_t      element_size;
} QUEUE;

void QUEUE_Ctor(QUEUE *me, void *buffer, uint8_t elements, size_t element_size);
void QUEUE_Post(QUEUE *me, void *element);
void QUEUE_Get(QUEUE *me, void *element);

#endif // __QUEUE_H_