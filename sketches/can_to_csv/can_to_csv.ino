/*
 * Built Using Seeed_Arduino_CAN example: https://github.com/Seeed-Studio/Seeed_Arduino_CAN/blob/master/examples/receive_monitor/receive_monitor.ino
 * 
 * CSV Format:
 *   frame_type, id, length, byte_1, byte_2, ... , byte_N (for N = length)
 * 
 * Displayed type:
 *   0x00: standard data frame
 *   0x02: extended data frame
 *   0x30: standard remote frame
 *   0x32: extended remote frame
 * 
 * Captured to CSV using Tera Term serial logging
 */
#include <SPI.h>
#include "mcp2515_can.h"

// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;

mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#define MAX_DATA_SIZE 8
#define BAUD_RATE CAN_500KBPS

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    while (CAN_OK != CAN.begin(BAUD_RATE)) {
        Serial.println(F("!CAN_Init_Fail;"));
        while(1);
    }
}

uint8_t  type; // bit0: ext, bit1: rtr
uint8_t  len;
byte cdata[MAX_DATA_SIZE] = {0};

void loop() {
    if (CAN_MSGAVAIL != CAN.checkReceive()) {
        return;
    }

    CAN.readMsgBuf(&len, cdata);

    type = (CAN.isExtendedFrame() << 0) |
           (CAN.isRemoteRequest() << 1);

    Serial.print(type); 
    Serial.print(",");

    Serial.print(CAN.getCanId());
    Serial.print(",");

    Serial.print(len);  // Message length

    // Print up to 8 bytes
    for (int i = 0; i < len; i++) {
        Serial.print(",");
        Serial.print(cdata[i]);
    }
    Serial.println();
}


// END FILE