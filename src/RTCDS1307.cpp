#include <RTCDS1307.h>

RTCDS1307::RTCDS1307(void) {
  _i2caddr = DS1307_ADDRESS;
  _clockreg = DS1307_CLOCKREG;
  _wdaybase = DS1307_WDAYBASE;
}

void RTCDS1307::init(void) {
  setRegister(0x07, 0b00000011); // typical value after power-on
  byte secs = getRegister(0x00);  // seconds register; high bit is the osc enabled bit
  if (secs & 0x80) setRegister(0x00, secs & 0x7F); // enable oscillator
}

bool RTCDS1307::isValid(void) {
  return (getRegister(0x00) & 0x80 == 0); // oscillator enabled
}

