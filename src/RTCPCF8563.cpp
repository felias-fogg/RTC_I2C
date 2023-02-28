#include <RTCPCF8563.h>

RTCPCF8563::RTCPCF8563(void) {
  _i2caddr = PCF8563_ADDRESS;
  _clockreg = PCF8563_CLOCKREG;
  _wdaybase = PCF8563_WDAYBASE;
  _wdayfirst = PCF8563_WDAYFIRST;
  _capabilities = PCF8563_CAPABIL;
}

void RTCPCF8563::init(void) {
  setRegister(PCF8563_CONTROL, 0);  
  setRegister(PCF8563_CONTROL+1, 0);
}

bool RTCPCF8563::isValid(void) {
  return ((getRegister(PCF8563_CONTROL) & 0b00010000) == 0) && // STOP not asserted 
    ((getRegister(PCF8563_CONTROL) & 0b10000000) == 0); // VL bit not asserted
}

void RTCPCF8563::enableAlarm(void) { 
  byte ctr = getRegister(PCF8563_CONTROL+1);
  setRegister(PCF8563_CONTROL+1, (ctr & 0b11111101) | 0b00000010);
}

void RTCPCF8563::disableAlarm(void) {
  byte ctr = getRegister(PCF8563_CONTROL+1);
  setRegister(PCF8563_CONTROL+1, (ctr & 0b11111101) | 0b00000000); 
}


void RTCPCF8563::enable32kHz(void) {
  setRegister(PCF8563_CLKOUT, 0b10000000); 
}

void RTCPCF8563::disable32kHz(void) {
  setRegister(PCF8563_CLKOUT, 0); 
}

void RTCPCF8563::enable1Hz(void) {
  setRegister(PCF8563_CLKOUT, 0b10000011); 
}

void RTCPCF8563::disable1Hz(void) {
  disable32kHz();
}

