#include <RTC_DS1307.h>

void DS1307::init(__attribute__ ((unused)) byte mode) {
  setRegister(DS1307_CONTROL, 0b00000011); // typical value after power-on
  byte secs = getRegister(DS1307_CLOCKREG);  // seconds register; high bit is the osc enabled bit
  if (secs & 0x80) setRegister(DS1307_CLOCKREG, secs & 0x7F); // enable oscillator
}

bool DS1307::isValid(void) {
  return ((getRegister(DS1307_CLOCKREG) & 0x80) == 0); // oscillator enabled
}

void DS1307::enable32kHz(void) {
  setRegister(DS1307_CONTROL, 0b00010011);
}

void DS1307::disable32kHz(void) {
  setRegister(DS1307_CONTROL, 0b00000000);
}

void DS1307::enable1Hz(void) {
  setRegister(DS1307_CONTROL, 0b00010000);
}

void DS1307::disable1Hz(void) {
  setRegister(DS1307_CONTROL, 0b00000000);
}

