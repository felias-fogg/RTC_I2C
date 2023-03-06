#include <RTC_RV3032.h>

void RV3032::init(byte mode) {
  setRegister(RV3032_CONTROL, 0); // clear control1 register
  setRegister(RV3032_CONTROL+1, 0); // clear control2 register
  setRegister(RV3032_CONTROL+2, 0); // clear control3 register
  setRegister(RV3032_CONTROL+3, 0); // clear time-stamp register
  setRegister(RV3032_CONTROL+4, 0); // clear clock interrupt mask register
  setRegister(RV3032_CONTROL+5, 0); // clear EVI control register
  setRegister(RV3032_CONTROL+6, 0); // clear temperature threshold register
  setRegister(RV3032_STATUS, 0);  // clear all flags
  setRegister(RV3032_CLKOUT, 0);  // 32 KHz output by default
  setRegister(RV3032_COE, 0b01000000 |
	      (mode == 0 ? 0 : (mode == 1 ? 0b100000 : 0b010000)));  // CLKOUT=off & switching mode
  updateEEPROMByte(RV3032_COE);
}

bool RV3032::isValid(void) {
  return ((getRegister(RV3032_STATUS) & 0b11) == 0); // both voltage low and POR flags are cleared
}

void RV3032::setAlarm(byte minute, byte hour) {
  setRegister(RV3032_ALARM, bin2bcd(minute)); // set minute alarm
  setRegister(RV3032_ALARM+1, bin2bcd(hour)); // set hour alarm
  setRegister(RV3032_ALARM+2, 0x80); // set weekday alarm to always
}


void RV3032::enableAlarm(void) { 
  byte ctr = getRegister(RV3032_CONTROL+1);
  setRegister(RV3032_CONTROL+1, ctr | 0b1000); // set the AIE bit
}

void RV3032::disableAlarm(void) {
  byte ctr = getRegister(RV3032_CONTROL+1);
  setRegister(RV3032_CONTROL+1, (ctr & 0b11110111));  // clear AIE bit
}

bool RV3032::senseAlarm(void) {
  return ((getRegister(RV3032_STATUS) & 0b1000) != 0);
}

void RV3032::clearAlarm(void) {
  byte ctr = getRegister(RV3032_STATUS);
  setRegister(RV3032_STATUS, (ctr & 0b11110111)); 
}


void RV3032::enable32kHz(void) {
  setRegister(RV3032_CLKOUT, 0); // set 32kHz
  updateEEPROMByte(RV3032_CLKOUT);
  setRegister(RV3032_COE, getRegister(RV3032_COE) & ~0b01000000); // enable CLKOUT
  updateEEPROMByte(RV3032_COE);
}

void RV3032::disable32kHz(void) {
  setRegister(RV3032_COE, getRegister(RV3032_COE) | 0b01000000); // disable CLKOUT
  updateEEPROMByte(RV3032_COE);
}

void RV3032::enable1Hz(void) {
  setRegister(RV3032_CLKOUT, 0b01100000); // set 1Hz
  updateEEPROMByte(RV3032_CLKOUT);
  setRegister(RV3032_COE, getRegister(RV3032_COE) & ~0b01000000); // enable CLKOUT
  updateEEPROMByte(RV3032_COE);
}

void RV3032::disable1Hz(void) {
  setRegister(RV3032_COE, getRegister(RV3032_COE) | 0b01000000); // disable CLKOUT
  updateEEPROMByte(RV3032_COE); 
}



// negative values make the clock faster by 0.2384 ppm/LSB
// The range of the internal parameter goes from -32 to +31.
// This means that possible values for offset range from -768 to +744 corresponding to -7.68 ppm to 7.44 ppm 
void RV3032::setOffset(int offset, byte mode) {
  if (mode != 2) {
    if (offset < 0) offset = offset - 12;
    else offset = offset + 12;
    offset = offset/24;
    if (offset < -32) offset = -32;
    else if (offset > 31) offset = 31;
  }
  //Serial.println(offset);
  //Serial.println(offset&0x3F);
  setRegister(RV3032_OFFSET, (offset&0x3F));
  updateEEPROMByte(RV3032_OFFSET);
}

  
void RV3032::updateEEPROMByte(byte reg) {
  byte cnts = getRegister(reg);
  setRegister(RV3032_CONTROL, getRegister(RV3032_CONTROL) | 0b100); // set EERD = 1
  setRegister(RV3032_EEADDR, reg);
  setRegister(RV3032_EEDATA, cnts);
  while (getRegister(RV3032_BUSY) & 0b100) { // busy with reading/writing EEPROM
    _delay_ms(2); // wait 10 ms
  }
  setRegister(RV3032_EECMD, 0x21); // update EEPROM at EEADDR with value stored in EEADDR
  while (getRegister(RV3032_BUSY) & 0b100) { // busy with reading/writing EEPROM
    _delay_ms(10); // wait 10 ms
  }
  setRegister(RV3032_CONTROL, getRegister(RV3032_CONTROL) & ~0b00000100); // set EERD = 0
}
