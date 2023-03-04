#include <RTC_RV3028.h>

void RV3028::init(byte mode) {
  setRegister(RV3028_CONTROL, 0); // clear control1 register
  setRegister(RV3028_CONTROL+1, 0); // clear control2 register
  setRegister(RV3028_STATUS, 0);  // clear all flags
  setRegister(RV3028_CLKOUT, 0b01000000);  // 32 KHz output by default, CLKOUT is off
  setRegister(RV3028_BSM, (getRegister(RV3028_BSM) & 0b11110011) |
	      (mode == 1 ? 0b1100 : (mode == 2 ? 0b0100 : 0)));  // switching mode
  updateEEPROMByte(RV3028_CLKOUT);
  updateEEPROMByte(RV3028_BSM);
}

bool RV3028::isValid(void) {
  return ((getRegister(RV3028_STATUS) & 0b1) == 0); // POR flag is cleared
}

void RV3028::setAlarm(byte minute, byte hour) {
  setRegister(RV3028_ALARM, bin2bcd(minute)); // set minute alarm
  setRegister(RV3028_ALARM+1, bin2bcd(hour)); // set hour alarm
  setRegister(RV3028_ALARM+2, 0x80); // set weekday/day alarm to always
}


void RV3028::enableAlarm(void) { 
  byte ctr = getRegister(RV3028_CONTROL+1);
  setRegister(RV3028_CONTROL+1, ctr | 0b1000); // set the AIE bit
}

void RV3028::disableAlarm(void) {
  byte ctr = getRegister(RV3028_CONTROL+1);
  setRegister(RV3028_CONTROL+1, (ctr & 0b11110111));  // clear AIE bit
}

bool RV3028::senseAlarm(void) {
  return ((getRegister(RV3028_STATUS) & 0b100) != 0);
}

void RV3028::clearAlarm(void) {
  byte ctr = getRegister(RV3028_STATUS);
  setRegister(RV3028_STATUS, (ctr & 0b11111011)); 
}


void RV3028::enable32kHz(void) {
  setRegister(RV3028_CLKOUT, 0b11000000); // enable CLKOUT 32kHz
  updateEEPROMByte(RV3028_CLKOUT);
}

void RV3028::disable32kHz(void) {
  setRegister(RV3028_CLKOUT, 0b01000000); // disable CLKOUT
  updateEEPROMByte(RV3028_CLKOUT);
}

// use nINT as output since the CLICK board does not
// support the output of CLKOUT
void RV3028::enable1Hz(void) {
  setRegister(RV3028_CONTROL+1, getRegister(RV3028_CONTROL) &  ~0b00010000); // USEL = 0  
  setRegister(RV3028_CONTROL+1, getRegister(RV3028_CONTROL+1) | 0b00100000); // UIE = 1
}

void RV3028::disable1Hz(void) {
  setRegister(RV3028_CONTROL+1, getRegister(RV3028_CONTROL+1) & ~0b00100000); // UIE = 0
}



// negative values make the clock faster by 0.9537 ppm/LSB
// The range of the internal parameter goes from -256 to +255.
// This means that possible values for offset range from -243.2 ppm to +244.1 ppm.
void RV3028::setOffset(int offset, __attribute__ ((unused)) byte mode) {
  if (offset < 0) offset = offset - 47;
  else offset = offset + 47;
  offset = offset/95;
  if (offset < -256) offset = -256;
  else if (offset > 255) offset = 255;
  Serial.println(offset);
  setRegister(RV3028_OFFSET, (offset>>1));
  setRegister(RV3028_OFFSET+1, (getRegister(RV3028_OFFSET+1) & 0b01111111) | ((offset & 1) << 7));
  updateEEPROMByte(RV3028_OFFSET);
  updateEEPROMByte(RV3028_OFFSET+1);
}

  
void RV3028::updateEEPROMByte(byte reg) {
  byte timeout = 0;
  byte cnts = getRegister(reg);
  setRegister(RV3028_CONTROL, getRegister(RV3028_CONTROL) | 0b1000); // set EERD = 1
  setRegister(RV3028_EEADDR, reg);
  setRegister(RV3028_EEDATA, cnts);
  while (++timeout && (getRegister(RV3028_STATUS) & 0b10000000)) { // busy with reading/writing EEPROM
    _delay_ms(20); // wait 2 ms
  }
  if (!timeout) {
    Serial.println(F("Timeout in EEPROM wait"));
    return;
  }
  timeout = 0;
  setRegister(RV3028_EECMD, 0x21); // update EEPROM at EEADDR with value stored in EEADDR
  while (++timeout && (getRegister(RV3028_STATUS) & 0b10000000)) { // busy with reading/writing EEPROM
    _delay_ms(10); // wait 10 ms
  }
  if (!timeout) {
    Serial.println(F("Timeout in EEPROM write"));
    return;
  }
  setRegister(RV3028_CONTROL, getRegister(RV3028_CONTROL) & ~0b00001000); // set EERD = 0
}
