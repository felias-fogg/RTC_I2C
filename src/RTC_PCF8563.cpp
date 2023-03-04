#include <RTC_PCF8563.h>


void PCF8563::init(__attribute__ ((unused)) byte mode) {
  setRegister(PCF8563_CONTROL, 0);  
  setRegister(PCF8563_CONTROL+1, 0);
  setRegister(PCF8563_CLKOUT, 0); 
}

bool PCF8563::isValid(void) {
  return ((getRegister(PCF8563_CONTROL) & 0b00010000) == 0) && // STOP not asserted 
    ((getRegister(PCF8563_CONTROL) & 0b10000000) == 0); // VL bit not asserted
}

void PCF8563::enableAlarm(void) { 
  byte ctr = getRegister(PCF8563_CONTROL+1);
  setRegister(PCF8563_CONTROL+1, (ctr & 0b11111101) | 0b00000010);
}

void PCF8563::disableAlarm(void) {
  byte ctr = getRegister(PCF8563_CONTROL+1);
  setRegister(PCF8563_CONTROL+1, (ctr & 0b11111101) | 0b00000000); 
}


void PCF8563::enable32kHz(void) {
  setRegister(PCF8563_CLKOUT, 0b10000000); 
}

void PCF8563::disable32kHz(void) {
  setRegister(PCF8563_CLKOUT, 0); 
}

void PCF8563::enable1Hz(void) {
  setRegister(PCF8563_CLKOUT, 0b10000011); 
}

void PCF8563::disable1Hz(void) {
  disable32kHz();
}

