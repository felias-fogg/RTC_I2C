#include <RTCDS1337.h>

RTCDS1337::RTCDS1337(void) {
  _i2caddr = DS1337_ADDRESS;
  _clockreg = DS1337_CLOCKREG;
  _wdaybase = DS1337_WDAYBASE;
  _wdayfirst = DS1337_WDAYFIRST;
  _capabilities = DS1337_CAPABIL;
}

void RTCDS1337::init(void) {
  setRegister(DS1337_CONTROL, 0b00000100); // typical value after power-on, except for bit 2 (disables SQW)
  setRegister(DS1337_STATUS, 0b00000000);  // clear OSF flag and clear alarm flags
}

bool RTCDS1337::isValid(void) {
  return ((getRegister(DS1337_STATUS) & 0x80) == 0); // OSF bit clerared = oscillator enabled
}

void RTCDS1337::enable32kHz(void) {
  byte ctr = getRegister(DS1337_CONTROL);
  setRegister(DS1337_CONTROL, (ctr & 0b11100011) | 0b00011000); // RS1=1 RS=1 INTCN=0
}

void RTCDS1337::disable32kHz(void) {
  byte ctr = getRegister(DS1337_CONTROL);
  setRegister(DS1337_CONTROL, (ctr & 0b11100011) | 0b00011100); // RS1=1 RS=1 INTCN=1
}

void RTCDS1337::enable1Hz(void) {
  byte ctr = getRegister(DS1337_CONTROL);
  setRegister(DS1337_CONTROL, (ctr & 0b11100011) | 0b00000000); // RS1=0 RS=0 INTCN=0
}

void RTCDS1337::disable1Hz(void) {
  disable32kHz();
}
