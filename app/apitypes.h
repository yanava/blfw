/* apitypes.h
 *
 *  Created on: Nov 27, 2009
 *      Author: jere
 */

#ifndef APITYPES_H_
#define APITYPES_H_

#include <stdint.h>

// Hardware Address Definition
typedef struct bd_addr_t
{
    uint8_t addr[6];
} bd_addr;

typedef bd_addr hw_addr;

// 8bit data array
typedef struct
{
    uint8_t len;
    uint8_t data[];
}uint8array;

// 16bit data array
typedef struct
{
    uint16_t len;
    uint8_t data[];
}uint16array;

// String array
typedef struct
{
    uint8_t len;
    int8_t  data[];
} string;

// IP Address Type
typedef union
{
        uint32_t u;
        uint8_t  a[4];
} ipv4;

#endif /* APITYPES_H_ */
