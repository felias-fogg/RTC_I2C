#include <RTC_RS5C372.h>

void RS5C372::init(__attribute__ ((unused)) byte mode) {
  setRegister(RS5C372_CONTROL1, 0b00100000); // INT(1Hz) and 32K to INTRB output, Alarm_A and Alarm_B to INTRA
  setRegister(RS5C372_CONTROL2, 0b00101000); // 24h format and 32K output disabled
  setRegister(RS5C372_OFFSET, 0); // 32.768 crystal and no trimming
}

bool RS5C372::isValid(void) {
  return ((getRegister(RS5C372_CONTROL2) & 0b00010000) == 0); // XSTP=0, otherwise clock is/was halted
}

void RS5C372::enableAlarm(void) { 
  byte ctr = getRegister(RS5C372_CONTROL1);
  setRegister(RS5C372_CONTROL1, (ctr | 0b10000000)); // set AALE bit
}

void RS5C372::disableAlarm(void) {
  byte ctr = getRegister(RS5C372_CONTROL1);
  setRegister(RS5C372_CONTROL1, (ctr & ~0b10000000));  // reset AALE bit
}

void RS5C372::setAlarm(byte minute, byte hour) {
  setRegister(RS5C372_ALARMMIN, bin2bcd(minute)); // set minute alarm
  setRegister(RS5C372_ALARMHR, bin2bcd(hour)); // set hour alarm
  setRegister(RS5C372_ALARMWDAYS, 0x7F); // set day alarm to always
}

bool RS5C372::senseAlarm(void) {
  return ((getRegister(RS5C372_CONTROL2) & 0b10) != 0);
}

void RS5C372::clearAlarm(void) {
  byte ctr = getRegister(RS5C372_CONTROL2);
  setRegister(RS5C372_CONTROL2, (ctr & 0b11111101)); 
}


void RS5C372::enable32kHz(void) {
  byte clkout = getRegister(RS5C372_CONTROL2);
  setRegister(RS5C372_CONTROL2, (clkout & ~0b00001000)); // set CLEN to 0
}

void RS5C372::disable32kHz(void) {
  byte clkout = getRegister(RS5C372_CONTROL2);
  setRegister(RS5C372_CONTROL2, (clkout | 0b00001000));  // set CLEN to 1
}

void RS5C372::enable1Hz(void) {
  byte clkout = getRegister(RS5C372_CONTROL1);
  setRegister(RS5C372_CONTROL1, (clkout & 0b11111000) | 0b00000011); 
}

void RS5C372::disable1Hz(void) {
  byte clkout = getRegister(RS5C372_CONTROL1);
  setRegister(RS5C372_CONTROL1, (clkout & 0b11111000)); 
}

void RS5C372::setOffset(int offset, byte mode) {
  if (mode != 2) {
    offset = (offset + (offset > 0 ? +152 : -152))/305;
    if (offset < -64) offset = -64;
    else if (offset > 63) offset = 63;
  }
  //Serial.println(offset);
  //Serial.println(((offset&0x7F)|(mode<<7)));
  setRegister(RS5C372_OFFSET, (offset&0x7F));
}

unsigned int RS5C372::getOffset(void) {
  return getRegister(RS5C372_OFFSET);
}
