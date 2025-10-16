Project to to record/playback, and mess around with CAN data for the purpose of decoding and controlling functionality of CAN bus devices. 

E.g. Controlling an 8th Gen Honda Civic Si tachometer, for various projects (e.g. sim racing, a clock, etc) via emulating Honda "F-CAN" bus. 

At its heart, this project is powered by an Arduino UNO R3 with a Seeed Studio CAN Shield V2.

This repo is a collection of scripts and testing sketches which can be used on any vehicle (accessible using pins on OBD2 port).

DISCLAIMER: I am not responsible for any damage to your vehicle by sending messages over an existing CAN bus.

Additional Notes: While this project does not deal with Single Wire CAN (such as Honda "B-CAN" or GMLAN) here's another cool project for SWCAN communications.
[SWCAN Board - Craig Peacock]()
