#include <RTC_PCF8523.h>

PCF8523::PCF8523(void) {
  _i2caddr = PCF8523_ADDRESS;
  _clockreg = PCF8523_CLOCKREG;
  _wdaybase = PCF8523_WDAYBASE;
  _wdayfirst = PCF8523_WDAYFIRST;
  _capabilities = PCF8523_CAP;
}

// if Vbat disabled, connect to Vcc
void PCF8523::init(byte mode) {
  setRegister(PCF8523_CONTROL, 0b00010000); // initiate power-on reset by software
  setRegister(PCF8523_CONTROL+2, (mode == 0 ?
				  0b01100000 :
				  (mode == 1 ? 0 : 0b00100000))); // switch mode and battery low detection
  setRegister(PCF8523_CONTROL+1, 0); // set default value for reg 1
  setRegister(PCF8523_CLKOUT, 0b00111000); // disable clock output
}

bool PCF8523::isValid(void) {
  return ((getRegister(PCF8523_CONTROL+2) & 0b00000100) == 0) && // no battery low flag and
    ((getRegister(PCF8523_CONTROL) & 0b00100000) == 0) && // oscillator is running
    ((getRegister(PCF8523_CLOCKREG) & 0b10000000) == 0);   // OS flag cleared
}

void PCF8523::enableAlarm(void) { 
  byte ctr = getRegister(PCF8523_CONTROL);
  setRegister(PCF8523_CONTROL, (ctr & 0b11111101) | 0b00000010);
}

void PCF8523::disableAlarm(void) {
  byte ctr = getRegister(PCF8523_CONTROL);
  setRegister(PCF8523_CONTROL, (ctr & 0b11111101) | 0b00000000); 
}


void PCF8523::enable32kHz(void) {
  byte clkout = getRegister(PCF8523_CLKOUT);
  setRegister(PCF8523_CLKOUT, (clkout & 0b11000111) | 0b00000000); 
}

void PCF8523::disable32kHz(void) {
  byte clkout = getRegister(PCF8523_CLKOUT);
  setRegister(PCF8523_CLKOUT, (clkout & 0b11000111) | 0b00111000); 
}

void PCF8523::enable1Hz(void) {
  byte clkout = getRegister(PCF8523_CLKOUT);
  setRegister(PCF8523_CLKOUT, (clkout & 0b11000111) | 0b00110000); 
}

void PCF8523::disable1Hz(void) {
  disable32kHz();
}

// negative values make the clock faster by roughly 4.0 ppm/LSB
// In mode 0 (correction every two hours), 1 LSB is roughly 4.34 ppm,
// in mode 1 (correction every minute), 1 LSB is roughly 4.06 ppm.
// The range goes from -64 to +63. 
void PCF8523::setOffset(int offset, byte mode) {
  if (mode == 0) 
    offset = (offset + (offset > 0 ? +217 : -217))/434;
  else
    offset = (offset + (offset > 0 ? +203 : -203))/406;
  if (offset < -64) offset = -64;
  else if (offset > 63) offset = 63;
  Serial.println(offset);
  Serial.println(((offset&0x7F)|(mode<<7)));
  setRegister(PCF8523_OFFSET, ((offset&0x7F)|(mode<<7)));
}
