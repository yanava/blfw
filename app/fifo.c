// FIFO.C
// Source file for GENERIC FIFO routines
// MAH - 06/2014 - Initial Version

#include <stdint.h>
#include <string.h>
#include "fifo.h"

// FIFO constructor 
void FIFO_Init(FIFO_T *me, void *buffer, uint8_t elements, size_t element_size)
{
    me->elements = elements;
    me->element_size = element_size;
    me->base = (uint8_t *) buffer;
    me->head = (uint8_t *) buffer;
    me->tail = (uint8_t *) buffer;    
}

// Puts new element on FIFO
void FIFO_Post(FIFO_T *me, void *element)
{ 
    // New head
    uint8_t *new_head = me->head + me->element_size;
    
    // Head wrapping around
    if (new_head > me->base + (me->elements*me->element_size) - 1)
        new_head = me->base;
     
    // Overflow, we should do something
    if (new_head == me->tail)
        return;
    // Adds a member to the FIFO and advance head
    else
    {
        memcpy(new_head,element,(size_t) me->element_size);  
        me->head = new_head;
    }  
}

// Gets an element from the FIFO
void FIFO_Get(FIFO_T *me, void *element)
{     
    // Tail caught up with head, nothing to do
    if (me->tail == me->head)
        return;
    // Gets the member from the FIFO and advance tail
    else
    {
        // New tail
        uint8_t *new_tail = me->tail + me->element_size;
    
        // Tail wrapping around
        if (new_tail > me->tail + (me->elements*me->element_size) - 1)
            new_tail = me->base;
        
        memcpy(element,new_tail,(size_t) me->element_size);  
        me->tail = new_tail;
    }  
}

// Gets the available space on the FIFO
// The minus one comes from the fact that Head == Tail is only acceptable as 
// a FIFO empty condition. So, for all practical purposes, you got one less
// space available to fill the Queue. 
uint8_t FIFO_AvailableSpace(FIFO_T *me)
{
    // Head == Tail -> Queue is free!
    if(me->head == me->tail)
        return (me->elements - 1);
    // Head is geometrically ahead of tail
    else if(me->head > me->tail)
        return (me->elements - 1 - (me->head - me->tail)/me->element_size);
    // Head is geometrically behind of tail
    else 
        return ((me->tail - me->head)/me->element_size - 1);
}

