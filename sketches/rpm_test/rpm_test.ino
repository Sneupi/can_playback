/*
2007 Honda Civic Si Tachometer RPM Test

Using Arduino Uno R3 with Seeed Studio CAN Shield V2

Tach powered via 12V (barrel jack) to its switched power pin (17) and ground (16)
Connected over tach FCAN High (1) and Low (19) pins

If you connected it right but do not see anything, try power cycling both uno/tach to reset CAN error mode.

The less duplicate frames you send in a row, the more reliable the display will be
*/

#include <SPI.h>
#include <mcp2515_can.h>

const int SPI_CS_PIN = 9;
mcp2515_can CAN(SPI_CS_PIN);

// #define USE_POT // potentiometer control
unsigned long lastUpdate = 0;
int i = 0;
unsigned long rpm_test = 0;

void send_useless() {
  // dummy frame of valid Honda something-or-other...
  byte data[8] = {6, 12, 0, 4, 20, 8, 0, 51};
  CAN.sendMsgBuf(660, 0, 8, data);
}

void send_rpm(unsigned long rpm) {
  rpm = (rpm * 0xFF) + 0xE;     // 0xFF multiplier, 0xE offset
  byte b1 = (rpm >> 16) & 0xFF; // most significant byte
  byte b2 = (rpm >> 8)  & 0xFF; // middle byte
  byte b3 = rpm & 0xFF;         // least significant byte
  bool extended = false;
  unsigned long id = 0x1DC;
  byte len = 4;
  byte data[8] = {2, b1,b2,b3};
  if (CAN.sendMsgBuf(id, extended, len, data) != CAN_OK) {
    Serial.println("Error: RPM Send");
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (CAN.begin(CAN_500KBPS) != CAN_OK) {
    Serial.println("CAN init failed!");
    while (1);
  }
  Serial.println("CAN init OK!");
}

void loop() {
  #if defined(USE_POT)

    unsigned long pot = (unsigned long)(((float)analogRead(A0)/(float)1023) * 9000);
    send_rpm(pot); // from potentiometer
    Serial.println(pot);

  #else

    unsigned long now = millis();
    send_rpm(rpm_test);
    Serial.println(rpm_test);
    
    if (now - lastUpdate > 10) { // more freq update = smoother readout
      lastUpdate = now;
      rpm_test = (rpm_test + 50) % 9000; // incr rpm
      if (rpm_test == 0) delay(2500); // wait for needle to reset
    }

  #endif
}
