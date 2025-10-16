/**
Instructions: load a sequence of known CAN frames into can_data, and program will play it back on-loop
*/

#include <SPI.h>
#include <mcp2515_can.h>
#include <avr/pgmspace.h>

// === PROGMEM startup array ===
// Format: {time_ms_since_last, extended(0/1), frame_id, len, byte1..byte8}
const unsigned long can_data[][12] PROGMEM = {
{3,0,660,8,6,12,0,4,20,8,0,51}, // valid honda frame (not sure what tho)
{3,0,476,4,2,0x02,0x66,0x2c,0,0,0,0}, // valid RPM 1 ~600
{3,0,660,8,6,12,0,4,20,8,0,51}, // valid honda frame (not sure what tho)
{3,0,476,4,2,0x02,0x66,0x1d,0,0,0,0}, // valid RPM 2 ~600
// {4,0,476,4,2,0x04,0xC5,0x25,0,0,0,0}, // valid RPM 3 ~1200
};

// Define NUM_FRAMES here
#define NUM_FRAMES (sizeof(can_data) / sizeof(can_data[0]))

// Define read helper here
unsigned long read_CAN_data(uint16_t row, uint8_t col) {
  return pgm_read_dword(&(can_data[row][col]));
}


// Set your SPI CS and INT pins (adjust if needed)
const int SPI_CS_PIN = 9;
mcp2515_can CAN(SPI_CS_PIN);

// === Sending logic ===
int currentFrame = 0;
unsigned long last = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (CAN.begin(CAN_500KBPS) != CAN_OK) {
    Serial.println("CAN init failed!");
    while (1);
  }
  Serial.println("CAN init OK!");
}


void send_from_progmem(int currentFrame) {
  // Read frame from PROGMEM
  bool extended = read_CAN_data(currentFrame, 1);
  unsigned long id = read_CAN_data(currentFrame, 2);
  byte len = read_CAN_data(currentFrame, 3);

  byte data[8] = {0};
  for (int i = 0; i < len && i < 8; i++) {
    data[i] = (byte)read_CAN_data(currentFrame, 4 + i);
  }

  // Send frame
  byte sendStatus = CAN.sendMsgBuf(id, extended, len, data);
  if (sendStatus == CAN_OK) {
    Serial.print("Sent frame ");
    Serial.print(currentFrame);
    Serial.print(" | ID: 0x");
    Serial.print(id, HEX);
    Serial.print(" | EXT: ");
    Serial.print(extended);
    Serial.print(" | LEN: ");
    Serial.print(len);
    Serial.print(" | DATA: ");
    for (int i = 0; i < len; i++) {
      if (data[i] < 0x10) Serial.print("0");
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  } else {
    Serial.print("Send failed at frame ");
    Serial.println(currentFrame);
  }
}


void loop() {
  unsigned long now = millis();
  if (now - last < 5) {
    return; // Wait
  }
  last = now;
  
  send_from_progmem(currentFrame);
  currentFrame++;

  if (currentFrame >= NUM_FRAMES) {
    currentFrame = 0;
  }
}
