#include "Arduino.h"
#include "stdio.h"
#include "Wire.h"

const uint8_t PCAaddress = 0B010000;

bool PCA9575DigitalRead(uint8_t pin);
bool PCA9575DigitalWrite(uint8_t pin, uint8_t value);
void PCA9575PinMode(uint8_t pin, uint8_t mode);
