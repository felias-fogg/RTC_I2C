#include <RTC_DS1337.h>

bool DS1337::init(__attribute__((unused)) BatteryMode mode) {
  bool success = setRegister(DS1337_CONTROL, 0b00000100); // typical value after power-on, except bit 2 disables SQW
  success &= setRegister(DS1337_STATUS, 0b00000000);      // clear OSF flag and clear alarm flags
  return success;
}

bool DS1337::isValid() {
  byte status;
  return readRegister(DS1337_STATUS, status) && ((status & 0x80) == 0); // OSF bit cleared = oscillator enabled
}

bool DS1337::enable32kHz() {
  byte ctr;
  if (!readRegister(DS1337_CONTROL, ctr)) return false;
  return setRegister(DS1337_CONTROL, (ctr & 0b11100011) | 0b00011000); // RS2=1 RS1=1 INTCN=0
}

bool DS1337::disable32kHz() {
  byte ctr;
  if (!readRegister(DS1337_CONTROL, ctr)) return false;
  return setRegister(DS1337_CONTROL, (ctr & 0b11100011) | 0b00011100); // RS2=1 RS1=1 INTCN=1
}

bool DS1337::enable1Hz() {
  byte ctr;
  if (!readRegister(DS1337_CONTROL, ctr)) return false;
  return setRegister(DS1337_CONTROL, (ctr & 0b11100011) | 0b00000000); // RS1=0 RS=0 INTCN=0
}

bool DS1337::disable1Hz() {
  return disable32kHz();
}

String DS1337::getManufacturer() {
  return F("Analog Devices");
}

String DS1337::getModel() {
  return F("DS1337");
}
