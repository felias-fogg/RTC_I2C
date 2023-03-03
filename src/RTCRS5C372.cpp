#include <RTCRS5C372.h>

RTCRS5C372::RTCRS5C372(void) {
  _i2caddr = RS5C372_ADDRESS;
  _clockreg = RS5C372_CLOCKREG;
  _wdaybase = RS5C372_WDAYBASE;
  _wdayfirst = RS5C372_WDAYFIRST;
  _capabilities = RS5C372_CAPABIL;
}

void RTCRS5C372::init(__attribute__ ((unused)) byte mode) {
  setRegister(RS5C372_CONTROL1, 0b00100000); // INT and 32K to INTRB output
  setRegister(RS5C372_CONTROL2, 0b00101000); // 24h format and 32K output disabled
  setRegister(RS5C372_OFFSET, 0); // 32.768 crystal and no trimming
}

bool RTCRS5C372::isValid(void) {
  return ((getRegister(RS5C372_CONTROL2) & 0b00010000) == 0); // XSTP=0, otherwise clock is/was halted
}

void RTCRS5C372::enableAlarm(void) { 
  byte ctr = getRegister(RS5C372_CONTROL1);
  setRegister(RS5C372_CONTROL1, (ctr | 0b10000000)); // set AALE bit
}

void RTCRS5C372::disableAlarm(void) {
  byte ctr = getRegister(RS5C372_CONTROL1);
  setRegister(RS5C372_CONTROL1, (ctr & ~0b10000000));  // reset AALE bit
}

void RTCRS5C372::setAlarm(byte minute, byte hour) {
  setRegister(RS5C372_ALARMMIN, bin2bcd(minute)); // set minute alarm
  setRegister(RS5C372_ALARMHR, bin2bcd(hour)); // set hour alarm
  setRegister(RS5C372_ALARMWDAYS, 0x7F); // set day alarm to always
}

bool RTCRS5C372::senseAlarm(void) {
  return ((getRegister(RS5C372_CONTROL2) & 0b10) != 0);
}

void RTCRS5C372::clearAlarm(void) {
  byte ctr = getRegister(RS5C372_CONTROL2);
  setRegister(RS5C372_CONTROL2, (ctr & 0b11111101)); 
}


void RTCRS5C372::enable32kHz(void) {
  byte clkout = getRegister(RS5C372_CONTROL2);
  setRegister(RS5C372_CONTROL2, (clkout & ~0b00001000)); // set CLEN to 0
}

void RTCRS5C372::disable32kHz(void) {
  byte clkout = getRegister(RS5C372_CONTROL2);
  setRegister(RS5C372_CONTROL2, (clkout | 0b00001000));  // set CLEN to 1
}

void RTCRS5C372::enable1Hz(void) {
  byte clkout = getRegister(RS5C372_CONTROL1);
  setRegister(RS5C372_CONTROL1, (clkout & 0b11111000) | 0b00000011); 
}

void RTCRS5C372::disable1Hz(void) {
  byte clkout = getRegister(RS5C372_CONTROL1);
  setRegister(RS5C372_CONTROL1, (clkout & 0b11111000)); 
}

void RTCRS5C372::setOffset(int offset, byte mode) {
  offset = (offset + (offset > 0 ? +152 : -152))/305;
  if (offset < -64) offset = -64;
  else if (offset > 63) offset = 63;
  Serial.println(offset);
  Serial.println(((offset&0x7F)|(mode<<7)));
  setRegister(RS5C372_OFFSET, (offset&0x7F));
}
