#include <nRF24L01.h>
#include <RF24.h>

//#define SerialOUTPUT
//-----------------------------PPM CONFIGURATION-----------------------------------
#define channel_number 8  //set the number of channels, in this case: 4 channels
#define sigPin 2          //set PPM signal output pin on the arduino
#define PPM_FrLen 22500   //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 300  //set the pulse length
//---------------------------------------------------------------------------------
#define clockMultiplier 2  // set this to 2 if you are using a 16MHz arduino, leave as 1 for an 8MHz arduino

int ppm[channel_number];
unsigned long lastRecvTime = 0;

RF24 radio(6, 7);  // CE, CSN
const uint64_t pipeIn = 0xE8E8F0F0E1LL;

struct PacketData {
  byte ch1;
  byte ch2;
  byte ch3;
  byte ch4;
  byte ch5;
  byte ch6;
  byte ch7;
  byte ch8;
};
PacketData receiverData;

void reset_received_Data() {
  receiverData.ch1 = 0;  //Throttle
  receiverData.ch2 = 127;
  receiverData.ch3 = 127;
  receiverData.ch4 = 127;
  receiverData.ch5 = 0;
  receiverData.ch6 = 0;
  receiverData.ch7 = 0;
  receiverData.ch8 = 0;

  ppm[0] = 1000;
  ppm[1] = 1500;
  ppm[2] = 1500;
  ppm[3] = 1500;
  ppm[4] = 1000;
  ppm[5] = 1000;
  ppm[6] = 1000;
  ppm[7] = 1000;
}


void setup() {
#ifdef SerialOUTPUT
  Serial.begin(9600);
#endif
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, 0);

  reset_received_Data();

  if (!radio.begin()) {
#ifdef SerialOUTPUT
    Serial.println("NRF24L01 not started");
#endif
    while (1)
      ;
  }
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  //radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1, pipeIn);
  radio.startListening();

  //Configure the interruption registers that will create the PPM signal
  cli();
  TCCR1A = 0;  // set entire TCCR1 register to 0
  TCCR1B = 0;

  OCR1A = 100;              // compare match register (not very important, sets the timeout for the first interrupt)
  TCCR1B |= (1 << WGM12);   // turn on CTC mode
  TCCR1B |= (1 << CS11);    // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  sei();
}

void loop() {
  if (radio.available()) {
    radio.read(&receiverData, sizeof(PacketData));
    ppm[0] = map(receiverData.ch1, 0, 255, 1000, 2000);
    ppm[1] = map(receiverData.ch2, 0, 255, 1000, 2000);
    ppm[2] = map(receiverData.ch3, 0, 255, 1000, 2000);
    ppm[3] = map(receiverData.ch4, 0, 255, 1000, 2000);
    ppm[4] = map(receiverData.ch5, 0, 1, 1000, 2000);
    ppm[5] = map(receiverData.ch6, 0, 1, 1000, 2000);
    ppm[6] = map(receiverData.ch7, 0, 1, 1000, 2000);
    ppm[7] = map(receiverData.ch8, 0, 1, 1000, 2000);

#ifdef SerialOUTPUT
    char inputValuesString[200];
    sprintf(inputValuesString, "%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d", ppm[0], ppm[1], ppm[2], ppm[3], ppm[4], ppm[5], ppm[6], ppm[7]);
    Serial.println(inputValuesString);
#endif

    lastRecvTime = millis();
  }

  unsigned long now = millis();
  if (now - lastRecvTime > 1000) {
    reset_received_Data();
#ifdef SerialOUTPUT
    Serial.println("Disconnected");
#endif
  }
}

//Here we create the PPM signal
ISR(TIMER1_COMPA_vect) {
  static boolean state = true;

  TCNT1 = 0;

  if (state) {
    //end pulse
    PORTD = PORTD & ~B00000100;  // turn pin 2 off. Could also use: digitalWrite(sigPin,0)
    OCR1A = PPM_PulseLen * clockMultiplier;
    state = false;
  } else {
    //start pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;

    PORTD = PORTD | B00000100;  // turn pin 2 on. Could also use: digitalWrite(sigPin,1)
    state = true;

    if (cur_chan_numb >= channel_number) {
      cur_chan_numb = 0;
      calc_rest += PPM_PulseLen;
      OCR1A = (PPM_FrLen - calc_rest) * clockMultiplier;
      calc_rest = 0;
    } else {
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * clockMultiplier;
      calc_rest += ppm[cur_chan_numb];
      cur_chan_numb++;
    }
  }
}