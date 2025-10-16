/*
Serial chunking protocol where serial sender waits for Arduino ack before send, 
thereby avoiding buffer lock-up of send while performing processes. 
Intakes line in chunks, performing actions once full line arrives.
*/

#define CHUNK_SIZE 64    // PC->Serial byte batch size
#define LINE_SIZE 100    // Size cap of serial line
#define ACK "!OK\n"      // Send ACK status on batch processed
#define READY "!READY\n" // Send READY status on startup

void setup() {
  Serial.begin(115200);
  Serial.flush();
  while (!Serial);
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

uint8_t line[LINE_SIZE];

void loop() {
  
  if (getLine(line)) {
    Serial.println((char*)line);
    // delay(100);
    line[0] = '\0'; // Reset line
  }

}
