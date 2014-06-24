// QUEUE.C
// Source file for GENERIC QUEUE routines
// MAH - 06/2014 - Initial Version

#include <stdint.h>
#include <string.h>
#include "queue.h"

void QUEUE_Ctor(QUEUE *me, void *buffer, uint8_t elements, size_t element_size)
{
    me->elements = elements;
    me->element_size = element_size;
    me->base = (uint8_t *) buffer;
    me->head = (uint8_t *) buffer;
    me->tail = (uint8_t *) buffer;    
}

// Puts new element on Queue
void QUEUE_Post(QUEUE *me, void *element)
{ 
    uint8_t *new_head = me->head + me->element_size;
    
    // Head wrapping around
    if (new_head > me->base + (me->elements*me->element_size) - 1)
        new_head = me->base;
     
    // Overflow, we should do something
    if (new_head == me->tail)
        return;
    // Adds a member to the Queue and advance head
    else
    {
        memcpy(new_head,element,(size_t) me->element_size);  
        me->head = new_head;
    }  
}

// Gets an element from the Queue
void QUEUE_Get(QUEUE *me, void *element)
{     
    // Tail caught up with head, nothing to do
    if (me->tail == me->head)
        return;
    // Gets the member from the Queue and advance tail
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