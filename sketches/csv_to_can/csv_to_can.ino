/*
Serial chunking protocol where serial sender waits for Arduino ack before send, 
thereby avoiding buffer lock-up of send while performing processes. 
Intakes line in chunks, performing actions once full line arrives.
*/

// CAN-BUS Shield, send data
// loovee@seeed.cc

#include <SPI.h>
#include "mcp2515_can.h"

// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin

#define CAN_BAUD CAN_500KBPS
#define SER_BAUD 115200
#define CHUNK_SIZE 64    // PC->Serial byte batch size
#define LINE_SIZE 100    // Size cap of serial line
#define ACK "!OK\n"      // Send ACK status on batch processed
#define READY "!READY\n" // Send READY status on startup
#define CAN_ERR "!CAN_ERR\n"

void setup() {
  Serial.begin(SER_BAUD);
  Serial.flush();
  while (!Serial);
  while (CAN_OK != CAN.begin(CAN_BAUD))
    delay(100);
  Serial.write(READY);
}

size_t readChunk(uint8_t *buffer) {
  static int bytesRead = 0; // static func var

  // Intake next batch
  size_t n = Serial.readBytes(buffer, min(Serial.available(), CHUNK_SIZE));
  buffer[n] = '\0';

  // Signal next batch once chunk taken
  bytesRead += n;
  if (bytesRead >= CHUNK_SIZE) {
    Serial.write(ACK);
    bytesRead = 0;
  }
  return n;
}

bool getLine(uint8_t *str) {
  // fill str with buf to exhaustion or ready
  while (Serial.available()) {    
    uint8_t buf[CHUNK_SIZE];
    readChunk(buf);

    // if end of line, return ready
    if (buf[0] == '\n' && buf[1] == '\0') 
      return true;
    else
      strncat(str, buf, LINE_SIZE);
  }
  return false;
}

void sendCAN(char *line) {
  char *tok = strtok(line, ",");
  uint8_t type = (uint8_t)atoi(tok);

  tok = strtok(NULL, ",");
  uint32_t id = (uint32_t)strtoul(tok, NULL, 0);

  tok = strtok(NULL, ",");
  uint8_t len = (uint8_t)atoi(tok);

  uint8_t bytes[8] = {0,0,0,0,0,0,0,0};
  for (uint8_t i = 0; i < len; ++i) {
    tok = strtok(NULL, ",");
    bytes[i] = (uint8_t)strtoul(tok, NULL, 0);
  }

  if (CAN_OK != CAN.sendMsgBuf(id, (type & 0b1), len, bytes)) {
    Serial.write(CAN_ERR);
  }
}

uint8_t line[LINE_SIZE];

void loop() {
  
  if (getLine(line)) {
    sendCAN(line);
    line[0] = '\0'; // Reset line
  }

}
