#include <RTC_DS1337.h>

void DS1337::init(__attribute__ ((unused)) byte mode) {
  setRegister(DS1337_CONTROL, 0b00000100); // typical value after power-on, except for bit 2 (disables SQW)
  setRegister(DS1337_STATUS, 0b00000000);  // clear OSF flag and clear alarm flags
}

bool DS1337::isValid(void) {
  return ((getRegister(DS1337_STATUS) & 0x80) == 0); // OSF bit clerared = oscillator enabled
}

void DS1337::enable32kHz(void) {
  byte ctr = getRegister(DS1337_CONTROL);
  setRegister(DS1337_CONTROL, (ctr & 0b11100011) | 0b00011000); // RS1=1 RS=1 INTCN=0
}

void DS1337::disable32kHz(void) {
  byte ctr = getRegister(DS1337_CONTROL);
  setRegister(DS1337_CONTROL, (ctr & 0b11100011) | 0b00011100); // RS1=1 RS=1 INTCN=1
}

void DS1337::enable1Hz(void) {
  byte ctr = getRegister(DS1337_CONTROL);
  setRegister(DS1337_CONTROL, (ctr & 0b11100011) | 0b00000000); // RS1=0 RS=0 INTCN=0
}

void DS1337::disable1Hz(void) {
  disable32kHz();
}
