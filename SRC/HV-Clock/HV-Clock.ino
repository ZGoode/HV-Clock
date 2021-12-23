/*
   1 switch for 24 hour time              0
   1 switch for settings                  1
   1 switch for daylight savings time     2
   1 button to switch menus               3
   1 button to add one in settings        4
   1 button to confirm selection          5
*/

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

void segWrite(uint64_t data);
void segSetVal(int mode);
void setByte(uint8_t index, uint8_t value);
void setOff(int index);
void setOn(int index);

bool blinking = false;
unsigned long lastBlink = 0;
int blinkInterval = 5000;
int blinkSettings = 500;

unsigned long lastPress = 0;
int pressInterval = 50;

unsigned long currentMillis = 0;

const int dataInPin = 2;
const int clockPin = 1;
const int dataPin = 3;
const int latchPin = 4;

int mode = 0;
int settingsMenu = 0;
int indexVal = 0;

int tempHour = 0;
int tempMinute = 0;
int tempSecond = 0;

int tempMonth = 0;
int tempDay = 0;
int tempYear = 0;

uint8_t shiftedIn = 0B00000000;

uint64_t sevSeg = 0B0000000000000000000000000000000000000000000000000000000000000000;

uint8_t segs[16] = {0B00000000, 0B00000001, 0B00000011, //0 1 2
                    0B00000111, 0B00001111, 0B00011111, //3 4 5
                    0B00111111, 0B01111111, 0B11111111, //6 7 8
                    0B11111110, 0B11111100, 0B11111000, //9 D A
                    0B11110000, 0B11100000, 0B11000000, //T E M
                    0B10000000                          //I
                   };

time_t tm = 0000000000;

//SDA PIN PB0
//SCL PIN PB2

void setup() {
  setSyncProvider(RTC.get);
  setSyncInterval(15);

  if (timeStatus() != timeSet) {
    setTime(tm);
    RTC.set(tm);
  }

  segWrite(sevSeg);

  noInterrupts();
}

void loop() {
  currentMillis = millis();

  if (currentMillis - lastBlink >= blinkInterval) {
    if (blinking) {
      bitSet(sevSeg, 15);
      bitSet(sevSeg, 31);
    } else {
      bitClear(sevSeg, 15);
      bitClear(sevSeg, 31);
    }

    blinking = ~blinking;
  }

  if (bitRead(shiftedIn, 1) == 0 && bitRead(shiftedIn, 3) == 1 && currentMillis - lastPress >= pressInterval) {
    if (mode == 0) {
      mode = 1;
    } else {
      mode = 0;
    }
    settingsMenu = 0;
    lastPress = currentMillis;
  }  else if (bitRead(shiftedIn, 1) == 1 && currentMillis - lastPress >= pressInterval && bitRead(shiftedIn, 3) == 1) {
    settingsMenu++;
    mode = 1 + settingsMenu;
    lastPress = currentMillis;

    indexVal = 0;

    if (mode == 3) {
      tempHour = hour();
      tempMinute = minute();
      tempSecond = second();
    } else if (mode == 5) {
      tempDay = day();
      tempMonth = month();
      tempYear = year();
    }

    if (mode == 4) {
      setTime(tempHour, tempMinute, tempSecond, day(), month(), year());
    }

    if (mode == 6) {
      setTime(hour(), minute(), second(), tempDay, tempMonth, tempYear);
    }

    if (mode >= 6) {
      settingsMenu = 1;
      mode = 1 + settingsMenu;
    }
  }

  if (bitRead(shiftedIn, 1) == 1 && bitRead(shiftedIn, 5) == 1 && currentMillis - lastPress >= pressInterval && mode == (3 || 5)) {
    lastPress = currentMillis;
    indexVal++;

    if (indexVal >= 6) {
      indexVal = 0;
    }
  }

  if (bitRead(shiftedIn, 4) == 1 && bitRead(shiftedIn, 1) == 1 && currentMillis - lastPress >= pressInterval && mode == (3 || 5)) {
    if (mode == 3) {
      switch (indexVal) {
        case 0:
          tempHour = tempHour + 10;
          break;
        case 1:
          tempHour = tempHour + 1;
          break;
        case 2:
          tempHour = tempMinute + 10;
          break;
        case 3:
          tempHour = tempMinute + 1;
          break;
        case 4:
          tempHour = tempSecond + 10;
          break;
        case 5:
          tempHour = tempSecond + 1;
          break;
      }
    } else if (mode == 5) {
      switch (indexVal) {
        case 0:
          tempMonth = tempMonth + 10;
          break;
        case 1:
          tempMonth = tempMonth + 1;
          break;
        case 2:
          tempDay = tempDay + 10;
          break;
        case 3:
          tempDay = tempDay + 1;
          break;
        case 4:
          tempYear = tempYear + 10;
          break;
        case 5:
          tempYear = tempYear + 1;
          break;
      }
    }
  }

  setSegVal(mode);
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
      break;
    case 1:
      if (day() < 10) {
        setByte(0, 0);
        setByte(1, day());
      } else {
        setByte(0, day() / 10);
        setByte(1, day() % 10);
      }

      if (month() < 10) {
        setByte(2, 0);
        setByte(3, month());
      } else {
        setByte(2, month() / 10);
        setByte(3, month() % 10);
      }

      if (year() % 2000 < 10) {
        setByte(4, 0);
        setByte(5, (year() % 2000));
      } else {
        setByte(4, (year() % 2000) / 10);
        setByte(5, (year() % 2000) % 10);
      }
      break;
    case 2:
      setOff(0);
      setByte(1, 12);
      setByte(2, 15);
      setByte(3, 14);
      setByte(4, 13);
      setOff(5);
      break;
    case 3:
      if (tempHour < 10) {
        setByte(0, 0);
        setByte(1, tempHour);
      } else {
        setByte(0, tempHour / 10);
        setByte(1, tempHour % 10);
      }

      if (tempMinute < 10) {
        setByte(2, 0);
        setByte(3, tempMinute);
      } else {
        setByte(2, tempMinute / 10);
        setByte(3, tempMinute % 10);
      }

      if (tempSecond < 10) {
        setByte(4, 0);
        setByte(5, tempSecond);
      } else {
        setByte(4, tempSecond / 10);
        setByte(5, tempSecond % 10);
      }

      if (currentMillis - lastBlink >= blinkSettings) {
        if (blinking) {
          setOff(indexVal);
        }

        blinking = ~blinking;
      }
      break;
    case 4:
      setOff(0);
      setByte(1, 10);
      setByte(2, 11);
      setByte(3, 12);
      setByte(4, 13);
      setOff(5);
      break;
    case 5:
      if (tempDay < 10) {
        setByte(0, 0);
        setByte(1, tempDay);
      } else {
        setByte(0, tempDay / 10);
        setByte(1, tempDay % 10);
      }

      if (tempMonth < 10) {
        setByte(2, 0);
        setByte(3, tempMonth);
      } else {
        setByte(2, tempMonth / 10);
        setByte(3, tempMonth % 10);
      }

      if (tempYear % 2000 < 10) {
        setByte(4, 0);
        setByte(5, (tempYear % 2000));
      } else {
        setByte(4, (tempYear % 2000) / 10);
        setByte(5, (tempYear % 2000) % 10);
      }

      if (currentMillis - lastBlink >= blinkSettings) {
        if (blinking) {
          setOff(indexVal);
        }

        blinking = ~blinking;
      }
      break;
  }
}

void setByte(uint8_t index, uint8_t value) {
  int current;

  for (int i = 0; i < 7; i++) {
    bitWrite(sevSeg, index * 8 + i, bitRead(segs[value], i));
  }
}

void setOff(int index) {
  for (int i = 0; i < 8; i++) {
    bitClear(sevSeg, index * 8 + i);
  }
}

void setOn(int index) {
  for (int i = 0; i < 8; i++) {
    bitSet(sevSeg, index * 8 + i);
  }
}

void shiftInput() {
  bitWrite(sevSeg, 7, 1);
  bitWrite(sevSeg, 15, 1);

  segWrite(sevSeg);
  delayMicroseconds(1);

  digitalWrite(latchPin, LOW);
  delayMicroseconds(1);
  digitalWrite(latchPin, HIGH);
  pinMode(dataInPin, INPUT);

  for (int i = 7; i >= 0; i--) {
    digitalWrite(clockPin, ~digitalRead(clockPin));
    delayMicroseconds(1);
    bitWrite(shiftedIn, i, digitalRead(dataInPin));
  }

  bitWrite(sevSeg, 7, 0);
  bitWrite(sevSeg, 15, 0);

  segWrite(sevSeg);
  delayMicroseconds(1);
}
