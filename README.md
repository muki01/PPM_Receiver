# PPM_Receiver

PPM_Receiver is a project that receives wireless data via the NRF24L01 module and outputs it as PPM signals. It is compatible with microcontrollers such as Arduino, ESP8266, and ESP32. It is designed for applications requiring PPM output, such as remote control systems or robotic control.

## ✨ Features
- Wireless data reception via NRF24L01
- Outputs PPM (Pulse Position Modulation) signals
- Simple and easy to use

## 🛠 Required Components
- Arduino, ESP8266, or ESP32
- NRF24L01 wireless module
- Jumper wires for connections

## 🔌 Wiring
1. Connect the NRF24L01 module to the microcontroller:
   - VCC -> 3.3V (Do not use 5V!)
   - GND -> GND
   - CE -> Pin 9
   - CSN -> Pin 10
   - SCK -> Pin 13 (May vary for ESP boards)
   - MOSI -> Pin 11
   - MISO -> Pin 12
2. Connect the PPM output pin to your desired device (e.g., servos, controllers).

## 🚀 Installation
1. Install the [RF24 Arduino library](https://github.com/nRF24/RF24) to use the NRF24L01 module.
2. Upload the `PPMReceiver.ino` sketch to your Arduino, ESP8266, or ESP32 board.
3. Open the Serial Monitor to check for received data.

## 📖 Usage
- Once uploaded and powered, the receiver will start receiving data wirelessly and output it as PPM signals.
- Ensure the transmitter is properly configured and paired with the receiver.

## 📸 Photos
Here are some images of the PPM Receiver example with Arduino Pro Mini 3.3v NRF24L01 SMD:

<img width=30% src="https://github.com/user-attachments/assets/f6f40827-df30-4f5d-8bba-07b861aa965d">
<img width=30% src="https://github.com/user-attachments/assets/bed6d445-acad-4f36-a112-281780b46d79">

