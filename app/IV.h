// IV.h
// Header file for I-V curve point extraction
// MAH - 07/2014 - Initial Version

#define IV_DEFAULT_POINT_DELAY (10)
#define IV_EVENT_LIST_SIZE     (10) 

void IV_Init(void) ;
void IV_Process(void);
void IV_Timertick (void);