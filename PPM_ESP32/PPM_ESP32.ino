#include <nRF24L01.h>
#include <RF24.h>
//#include <SPI.h>
RF24 radio(6, 7);
const uint64_t pipeIn = 0xE8E8F0F0E1LL;

#define PPM_FRAME_LENGTH 22500
#define PPM_PULSE_LENGTH 300
#define PPM_CHANNELS 8
#define DEFAULT_CHANNEL_VALUE 1500
#define OUTPUT_PIN 1

unsigned long lastRecvTime = 0;

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

uint16_t channelValue[PPM_CHANNELS] = { 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500 };

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

enum ppmState_e {
  PPM_STATE_IDLE,
  PPM_STATE_PULSE,
  PPM_STATE_FILL,
  PPM_STATE_SYNC
};

void IRAM_ATTR onPpmTimer() {

  static uint8_t ppmState = PPM_STATE_IDLE;
  static uint8_t ppmChannel = 0;
  static uint8_t ppmOutput = LOW;
  static int usedFrameLength = 0;
  int currentChannelValue;

  portENTER_CRITICAL(&timerMux);

  if (ppmState == PPM_STATE_IDLE) {
    ppmState = PPM_STATE_PULSE;
    ppmChannel = 0;
    usedFrameLength = 0;
    ppmOutput = LOW;
  }

  if (ppmState == PPM_STATE_PULSE) {
    ppmOutput = HIGH;
    usedFrameLength += PPM_PULSE_LENGTH;
    ppmState = PPM_STATE_FILL;

    timerAlarmWrite(timer, PPM_PULSE_LENGTH, true);
  } else if (ppmState == PPM_STATE_FILL) {
    ppmOutput = LOW;
    currentChannelValue = channelValue[ppmChannel];

    ppmChannel++;
    ppmState = PPM_STATE_PULSE;

    if (ppmChannel >= PPM_CHANNELS) {
      ppmChannel = 0;
      timerAlarmWrite(timer, PPM_FRAME_LENGTH - usedFrameLength, true);
      usedFrameLength = 0;
    } else {
      usedFrameLength += currentChannelValue - PPM_PULSE_LENGTH;
      timerAlarmWrite(timer, currentChannelValue - PPM_PULSE_LENGTH, true);
    }
  }
  portEXIT_CRITICAL(&timerMux);
  digitalWrite(OUTPUT_PIN, ppmOutput);
}

void reset_received_Data() {
  receiverData.ch1 = 0;  //Throttle
  receiverData.ch2 = 127;
  receiverData.ch3 = 127;
  receiverData.ch4 = 127;
  receiverData.ch5 = 0;
  receiverData.ch6 = 0;
  receiverData.ch7 = 0;
  receiverData.ch8 = 0;

  channelValue[0] = 1000;
  channelValue[1] = 1500;
  channelValue[2] = 1500;
  channelValue[3] = 1500;
  channelValue[4] = 1000;
  channelValue[5] = 1000;
  channelValue[6] = 1000;
  channelValue[7] = 1000;
}

void setup() {
  Serial.begin(115200);
  //SPI.begin(8, 5, 10, 7);  // SCK=8, MISO=5, MOSI=10, CSN=7
  pinMode(OUTPUT_PIN, OUTPUT);

  reset_received_Data();

  if (!radio.begin()) {
    Serial.println("NRF24L01 hatası");
    while (1)
      ;
  }

  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, pipeIn);
  radio.startListening();

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onPpmTimer, true);
  timerAlarmWrite(timer, 12000, true);
  timerAlarmEnable(timer);
}

void loop() {
  if (radio.available()) {
    radio.read(&receiverData, sizeof(PacketData));
    channelValue[0] = map(receiverData.ch1, 0, 255, 1000, 2000);
    channelValue[1] = map(receiverData.ch2, 0, 255, 1000, 2000);
    channelValue[2] = map(receiverData.ch3, 0, 255, 1000, 2000);
    channelValue[3] = map(receiverData.ch4, 0, 255, 1000, 2000);
    channelValue[4] = map(receiverData.ch5, 0, 1, 1000, 2000);
    channelValue[5] = map(receiverData.ch6, 0, 1, 1000, 2000);
    channelValue[6] = map(receiverData.ch7, 0, 1, 1000, 2000);
    channelValue[7] = map(receiverData.ch8, 0, 1, 1000, 2000);

    Serial.printf("Channel Values: %d %d %d %d %d %d %d %d\n", channelValue[0], channelValue[1], channelValue[2], channelValue[3], channelValue[4], channelValue[5], channelValue[6], channelValue[7]);

    lastRecvTime = millis();
  }

  unsigned long now = millis();
  if (now - lastRecvTime > 1000) {
    reset_received_Data();
    Serial.println("Disconnected");
  }
}
