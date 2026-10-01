#include <RTC_DS1307.h>

bool DS1307::init(__attribute__((unused)) BatteryMode mode) {
  bool success = true;
  success &= setRegister(DS1307_CONTROL, 0b00000011); // typical value after power-on
  byte secs;
  if (!readRegister(DS1307_CLOCKREG, secs)) return false; // seconds register; high bit is the oscillator halt bit
  if (secs & 0x80) success &= setRegister(DS1307_CLOCKREG, secs & 0x7F); // enable oscillator
  return success;
}

bool DS1307::isValid() {
  byte seconds;
  bool success = readRegister(DS1307_CLOCKREG, seconds);
  return success && ((seconds & 0x80) == 0); // oscillator enabled
}

bool DS1307::enable32kHz() {
  return setRegister(DS1307_CONTROL, 0b00010011);
}

bool DS1307::disable32kHz() {
  return setRegister(DS1307_CONTROL, 0b00000000);
}

bool DS1307::enable1Hz() {
  return setRegister(DS1307_CONTROL, 0b00010000);
}

bool DS1307::disable1Hz() {
  return setRegister(DS1307_CONTROL, 0b00000000);
}

String DS1307::getManufacturer() {
  return F("Analog Devices");
}

String DS1307::getModel() {
  return F("DS1307");
}
