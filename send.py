import serial
import time
import os
import sys

# CONFIG
CHUNK_SIZE = 64        # bytes/packet (also max Arduino serial buf)
STATUS = b'!'
ACK = STATUS + b'OK'   # Arduino sends this after each packet (use bytes for comparison)
READY = STATUS + b'READY'
TIMEOUT = 2

def send_file(filename, delay_ms, echo):
    filesize = os.path.getsize(filename)
    sent = 0

    with serial.Serial(PORT, BAUD, timeout=TIMEOUT) as ser:
        
        print(f"Connected to {PORT} at {BAUD} baud")
        print(f"Waiting for signal {READY}")
        
        while ser.readline().strip() != READY:
            pass
    
        print(f"Transmitting \"{filename}\" {f' with line delay {delay_ms} ms' if delay_ms else ''}")

        with open(filename, 'rb') as f:
            # Do until EOF
            while True:
                chunk = f.read(CHUNK_SIZE)
                if not chunk:
                    break

                # send data chunk
                ser.write(chunk)
                ser.flush()

                # wait for ACK from Arduino
                ack = ser.readline().strip()
                if ack != ACK and len(chunk) == CHUNK_SIZE:
                    print("ERROR, Bad or no ack: ", ack)
                    break
                    
                # Print sent updates
                sent += len(chunk)
                if echo:
                    print(chunk.decode('utf-8', errors='replace'), end='')
                else:
                    print(f"Sent {sent}/{filesize} bytes ({sent/filesize*100:.1f}%)", end='\r')
                
                # wait delay
                if delay_ms > 0:
                    time.sleep(delay_ms / 1000)

        print("\nDone sending file!")
        ser.write(b'DONE\n')
    


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python send.py <FILENAME> <PORT> <BAUD> [--echo] [--delay=<ms>]")
        sys.exit(1)

    FILENAME = sys.argv[1]
    PORT = sys.argv[2]
    BAUD = int(sys.argv[3])
    ECHO = False
    DELAY_MS = 0

    for arg in sys.argv[4:]:
        if arg == "--echo":
            ECHO = True
        elif arg.startswith("--delay="):
            DELAY_MS = int(arg.split("=")[1])
            
    try:        
        send_file(FILENAME, DELAY_MS, ECHO)  # change file as needed
    except FileNotFoundError:
        print(f"File not found: {FILENAME}")
    except KeyboardInterrupt:
        print("\nTransmission interrupted.")