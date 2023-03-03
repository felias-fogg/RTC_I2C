#include <RTCRV3032.h>

RTCRV3032::RTCRV3032(void) {
  _i2caddr = RV3032_ADDRESS;
  _clockreg = RV3032_CLOCKREG;
  _wdaybase = RV3032_WDAYBASE;
  _wdayfirst = RV3032_WDAYFIRST;
  _capabilities = RV3032_CAPABIL;
}

void RTCRV3032::init(byte mode) {
  setRegister(RV3032_CONTROL, 0); // clear control1 register
  setRegister(RV3032_CONTROL+1, 0); // clear control2 register
  setRegister(RV3032_CONTROL+2, 0); // clear control3 register
  setRegister(RV3032_CONTROL+3, 0); // clear time-stamp register
  setRegister(RV3032_CONTROL+4, 0); // clear clock interrupt mask register
  setRegister(RV3032_CONTROL+5, 0); // clear EVI control register
  setRegister(RV3032_CONTROL+6, 0); // clear temperature threshold register
  setRegister(RV3032_BSM, (mode == 0 ? 0 : (mode == 1 ? 0b1100 : 0b0100))); // battery switching
  setRegister(RV3032_STATUS, 0);  // clear all flags
  setRegister(RV3032_CLKOUT, 0);  // 32 KHz output by default
  setRegister(RV3032_COE, 0b01010000);  // CLKOUT=off, direct switching mode
  
}

bool RTCRV3032::isValid(void) {
  return ((getRegister(RV3032_STATUS) & 0b11) == 0); // both voltage low and POR flags are cleared
}

void RTCRV3032::setAlarm(byte minute, byte hour) {
  setRegister(RV3032_ALARM, bin2bcd(minute)); // set minute alarm
  setRegister(RV3032_ALARM+1, bin2bcd(hour)); // set hour alarm
  setRegister(RV3032_ALARM+2, 0x80); // set weekday alarm to always
}


void RTCRV3032::enableAlarm(void) { 
  byte ctr = getRegister(RV3032_CONTROL+1);
  setRegister(RV3032_CONTROL+1, ctr | 0b1000); // set the AIE bit
}

void RTCRV3032::disableAlarm(void) {
  byte ctr = getRegister(RV3032_CONTROL+1);
  setRegister(RV3032_CONTROL+1, (ctr & 0b11110111));  // clear AIE bit
}

bool RTCRV3032::senseAlarm(void) {
  return ((getRegister(RV3032_STATUS) & 0b1000) != 0);
}

void RTCRV3032::clearAlarm(void) {
  byte ctr = getRegister(RV3032_STATUS);
  setRegister(RV3032_STATUS, (ctr & 0b11110111)); 
}


void RTCRV3032::enable32kHz(void) {
  setRegister(RV3032_CLKOUT, 0); // set 32kHz
  setRegister(RV3032_COE, getRegister(RV3032_COE) & ~0b01000000); // enable CLKOUT
}

void RTCRV3032::disable32kHz(void) {
  setRegister(RV3032_COE, getRegister(RV3032_COE) | 0b01000000); // disable CLKOUT
}

void RTCRV3032::enable1Hz(void) {
  setRegister(RV3032_CLKOUT, 0b01100000); // set 1Hz
  setRegister(RV3032_COE, getRegister(RV3032_COE) & ~0b01000000); // enable CLKOUT
}

void RTCRV3032::disable1Hz(void) {
  setRegister(RV3032_COE, getRegister(RV3032_COE) | 0b01000000); // disable CLKOUT
}



// negative values make the clock faster by 0.2384 ppm/LSB
// The range of the internal parameter goes from -32 to +31.
// This means that possible values for offset range from -768 to +744 corresponding to -7.68 ppm to 7.44 ppm 
void RTCRV3032::setOffset(int offset, __attribute__ ((unused)) byte mode) {
  if (offset < 0) offset = offset - 12;
  else offset = offset + 12;
  offset = offset/24;
  if (offset < -32) offset = -32;
  else if (offset > 31) offset = 31;
  Serial.println(offset);
  Serial.println(offset&0x3F);
  setRegister(RV3032_OFFSET, (offset&0x3F));
}

  
