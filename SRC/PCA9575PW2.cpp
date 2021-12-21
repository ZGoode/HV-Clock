#include "PCA9575PW2.h"

uint16_t writeMode = 0B0000000000000000;
uint16_t readMode = 0B0000000000000000;
uint16_t byteBuffered = 0B0000000000000000;
uint16_t writeByteBuffered = 0B0000000000000000;

bool PCA9575DigitalRead(uint8_t pin, uint8_t register) {
  Wire.beginTransmission(PCAddress);
  Wire.write(register);
  Wire.requestFrom(PCAaddress, (uint8_t)1);
  if (Wire.available()) {
    byte iInput = Wire.read();
    if ((iInput & readMode) > 0) {
      byteBuffered = byteBuffered | (uint16_t)iInput;
    }
  }

  if ((bit(pin) & byteBuffered) > 0) {
    return true;
  }

  return false;
}

bool PCA9575DigitalWrite(uint8_t pin, uint8_t value, uint8_t register) {
  Wire.beginTransmission(PCAaddress);
  Wire.write(register);

  if (value == HIGH) {
    writeByteBuffered = writeByteBuffered | bit(pin);
  } else {
    writeByteBuffered = writeByteBuffered & ~bit(pin);
  }

  writeByteBuffered = writeByteBuffered & writeMode;
  Wire.write(writeByteBuffered);
  Wire.endTransmission();
}

void PCA9575PinMode(uint8_t pin, uint8_t mode) {
  if (mode == OUTPUT) {
    writeMode = writeMode | bit(pin);
    readMode = readMode | ~bit(pin);
  } else if (mode == INPUT) {
    writeMode = writeMode | ~bit(pin);
    readMode = readMode | bit(pin);
  }
}
