#include "wifi_bglib.h"
#include "usart.h"
#include "fsm.h"
#include <stdint.h>

#define WF121_EVENT_LIST_SIZE       (10)
#define WF121_SCAN_CHANNELS         (11)
#define WF121_BSSID_SIZE            (6)

// Define the use of BGLIB
BGLIB_DEFINE()

// Event type, parameters can be added after super
typedef struct WF121_EVENT_TAG
{
    FSM_Event super;
} WF121_EVENT_T;

// Event FIFO type
typedef struct WF121_EVENT_LIST_TAG
{
    FIFO_T     super;
    WF121_EVENT_T list[WF121_EVENT_LIST_SIZE];
} WF121_EVENT_LIST_T;

enum WF121_SIGNALS
{ 
    WF121_SEND_DATA,
    WF121_RECV_DATA
};

typedef struct WF121_TAG
{
    FSM super;
    uint8_t bssid[WF121_BSSID_SIZE];
    int16_t rssi;
    uint8_t tcp_endpoint;
    uint32_t dest_ip;
    uint16_t dest_port;
} WF121_T;

static uint8_t scan_channels[WF121_SCAN_CHANNELS] = {1,2,3,4,5,6,7,8,9,10,11};

static WF121_T wifiman;

// Initial state. Just performs the initial transition
FSM_State WF121_HAND_INITIAL(WF121_T *me, FSM_Event *e)
{
    me->rssi = 0;
    me->rx_buf_ptr = 0;
    me->dest_ip = 0;
    me->dest_port = 0;
    
    // Goes to IDLE mode
    return FSM_TRAN(me,NULL);
}

FSM_State WF121_HAND_IDLE(WF121_T *me, FSM_Event *e)
{
    switch (e->signal)
    {
        case FSM_ENTRY_SIGNAL:
            return FSM_HANDLED();
        case FSM_EXIT_SIGNAL:
            return FSM_HANDLED();   
    }
    
    // Default: Handled
    return FSM_HANDLED();
}

// Send function for the WF121
static void WF121_Send(uint8_t msg_len, uint8_t* msg_data, uint16_t data_len, uint8_t* data)
{
    // Sends header
    USART2_Send_Packet(msg_len, msg_data);
    
    // If there's any data, send data
    if(data_len && data) 
        USART2_Send_Packet(data_len, data);
}


// Test Program for the WF121 Chip
void WF121_Test()
{

    // Initialize BGLIB with Send function for communicating with the WF121
    BGLIB_INITIALIZE(WF121_Send);

    wifi_cmd_system_hello();
}
