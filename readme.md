# CAN Stream Recorder and Playback Tool for Arduino

Project to record, playback, and pipe CSV files containing CAN data to CAN bus for the purpose of decoding and controlling CAN devices. 

At its heart, this project is powered by an Arduino UNO R3 with a Seeed Studio CAN Shield V2, connected to a PC over serial.

## Record:
- Load `/sketches/can_to_csv/can_to_csv.ino`
- Connect shield to CAN bus
- Connect to terminal emulator that supports logging (e.g. Tera Term) 
- Log terminal output to CSV
- Power up CAN device under test

## Playback:
- Load `/sketches/csv_to_can/csv_to_can.ino`
- Run `send.py` with a formatted CSV (see `/data`)

## Related Projects:
1. [8th_Civic_Tachometer - Sneupi](https://github.com/Sneupi/8th_civic_tachometer) - Controlling an 8th Gen Honda Civic Si tachometer, for various projects (e.g. sim racing, a clock, etc) via emulating Honda "F-CAN" bus found within the OBD2 plug. 
2. [SWCAN Board - Craig Peacock](https://www.beyondlogic.org/swcan-single-wire-can-transceiver-breakout-board/) - Talking with Single Wire CAN (such as Honda "B-CAN", GMLAN, Tesla non-NACS EV chargers)

## DISCLAIMER: 
I am not responsible for any damage to devices you may send messages to.
