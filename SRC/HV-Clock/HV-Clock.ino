#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

void segWrite(uint64_t data);
void segSetVal(int mode);
void setByte(uint8_t index, uint8_t value);
void setOff(int index);
void setOn(int index);

bool blinking = false;
long lastBlink = 0;
int blinkInterval = 5000;

const int clockPin = 1;
const int dataPin = 3;
const int latchPin = 4;

uint64_t sevSeg = 0B0000000000000000000000000000000000000000000000000000000000000000;

uint8_t segs[10] = {0B00000000, 0B00000001, 0B00000011,
                    0B00000111, 0B00001111, 0B00011111,
                    0B00111111, 0B01111111, 0B11111111,
                    0B11111110
                   };

time_t tm = 0000026400;

//SDA PIN PB0
//SCL PIN PB2

void setup() {
  setSyncProvider(RTC.get);

  if (timeStatus() != timeSet) {
    setTime(tm);
    RTC.set(tm);
  }

  segWrite(sevSeg);
}

void loop() {
  setSegVal(0);
  segWrite(sevSeg);
}

void segWrite(uint64_t data) {
  digitalWrite(clockPin, LOW);
  digitalWrite(latchPin, LOW);

  for (int i = 0; i < 48; i++) {
    digitalWrite(clockPin, ~digitalRead(clockPin));
    digitalWrite(dataPin, bitRead(data, i));
  }

  digitalWrite(latchPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(latchPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);
}

void setSegVal(int mode) {
  switch (mode) {
    case 0:
      for (int i = 0; i < 48; i++) {
        if (hour() < 10) {
          setByte(0, 0);
          setByte(1, hour());
        } else {
          setByte(0, hour() / 10);
          setByte(1, hour() % 10);
        }

        if (minute() < 10) {
          setByte(2, 0);
          setByte(3, minute());
        } else {
          setByte(2, minute() / 10);
          setByte(3, minute() % 10);
        }

        if (second() < 10) {
          setByte(4, 0);
          setByte(5, second());
        } else {
          setByte(4, second() / 10);
          setByte(5, second() % 10);
        }
      }
      break;
    case 1:
      break;
  }
}

void setByte(uint8_t index, uint8_t value) {
  int current;

  switch (value) {
    case 0:
      current = 0;
      break;
    case 1:
      current = 1;
      break;
    case 2:
      current = 2;
      break;
    case 3:
      current = 3;
      break;
    case 4:
      current = 4;
      break;
    case 5:
      current = 5;
      break;
    case 6:
      current = 6;
      break;
    case 7:
      current = 7;
      break;
    case 8:
      current = 8;
      break;
    case 9:
      current = 9;
      break;
  }

  for (int i = 0; i < 7; i++) {
    bitWrite(sevSeg, index * 8 + i, bitRead(segs[current], i));
  }
}

void setOff(int index) {
  for (int i = 0; i < 8; i++) {
    bitWrite(sevSeg, index * 8 + i, 0);
  }
}

void setOn(int index) {
  for (int i = 0; i < 8; i++) {
    bitWrite(sevSeg, index * 8 + i, 1);
  }
}
