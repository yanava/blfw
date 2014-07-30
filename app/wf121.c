// wf121.c
// Source file for wi-fi module
// MAH - 07/2014 - Initial Version

#include <stdint.h>
#include "wifi_bglib.h"
#include "usart.h"

BGLIB_DEFINE();


/**
 * Configurable parameters that can be modified to match the test setup.
 */

/** The default destination TCP port. */
uint16_t default_dest_port = 80;
/** The default destination GET string. */
uint8_t* default_dest_get = "GET / HTTP1.0\r\n\r\n";

/** List of channels to scan. */
static const uint8_t scan_channels[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
/** BSSID to connect to. */
static uint8_t selected_bssid[6];
/** Signal strength of the selected BSSID. */
static int16_t selected_rssi = 0;
/** TCP endpoint. */
static uint8_t tcp_endpoint;
/** Buffer for received data. */
static uint8_t recv_buffer[4096];
/** Index to receive buffer. */
static uint16_t recv_buffer_idx = 0;
/** The serial port to use for BGAPI communication. */
static uint8_t* uart_port = NULL;
/** The baud rate to use. */
static uint32_t baud_rate = 0;
/** The destination IP address */
static uint32_t dest_ip = 0;
/** The destination TCP port. */
static uint16_t dest_port = 0;



// Writes messages to the WF121 Serial Port
void WF121_MessageSend(uint8_t msg_len, uint8_t* msg_data, uint16_t data_len, uint8_t* data)
{   
    USART2_Send_Packet(msg_len,msg_data);
    
    if(data_len && data) 
        USART2_Send_Packet(data_len,data);

}

void WF121_Init(void)
{
    BGLIB_INITIALIZE(WF121_MessageSend);
}



