# PPMReceiver

PPMReceiver is an Arduino project that receives wireless data via the NRF24L01 module and outputs it as PPM signals. It is designed for use in applications such as remote control systems or robotic control, where PPM output is needed.

## Features
- Wireless data reception via NRF24L01
- Outputs PPM (Pulse Position Modulation) signals
- Simple and easy to use

## Requirements
- Arduino Pro Mini (or any compatible Arduino board)
- NRF24L01 wireless module
- Jumper wires for connections

## Wiring
1. Connect the NRF24L01 module to the Arduino:
   - VCC -> 3.3V (Do not use 5V!)
   - GND -> GND
   - CE -> Pin 9
   - CSN -> Pin 10
   - SCK -> Pin 13
   - MOSI -> Pin 11
   - MISO -> Pin 12
2. Connect the PPM output pin to your desired device (e.g., servos, controllers).

## Installation
1. Install the [RF24 Arduino library](https://github.com/nRF24/RF24) to use the NRF24L01 module.
2. Upload the `PPMReceiver.ino` sketch to your Arduino board.
3. Open the Serial Monitor to check for received data.

## Usage
- Once uploaded and powered, the receiver will start receiving data wirelessly and output it as PPM signals.
- Ensure the transmitter is properly configured and paired with the receiver.

## Photos
Here are some images of the PPMReceiver setup:

![Receiver Setup]()
*Receiver setup on Arduino Pro Mini with NRF24L01 module.*