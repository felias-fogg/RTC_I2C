#include <RTC_RV8803.h>

void RV8803::init(__attribute__ ((unused)) byte mode) {
  setRegister(RV8803_CONTROL, 0); // clear control register
  setRegister(RV8803_STATUS, 0);  // clear all flags
  setRegister(RV8803_CLKOUT, 0);  // 32 KHz output by default
}

bool RV8803::isValid(void) {
  return ((getRegister(RV8803_STATUS) & 0b11) == 0); // both voltage low flags are cleared
}

// set & clear reset bit when setting time/date
void RV8803::setTime(time_t t) {
  setRegister(RV8803_CONTROL, getRegister(RV8803_CONTROL) | 1); // set RESET bit
  RTC::setTime(t);
  setRegister(RV8803_CONTROL, getRegister(RV8803_CONTROL) & 0b11111110); // clear RESET bit  
}

void RV8803::setTime(tmElements_t tm) {
  setRegister(RV8803_CONTROL, getRegister(RV8803_CONTROL) | 1); // set RESET bit
  RTC::setTime(tm);
  setRegister(RV8803_CONTROL, getRegister(RV8803_CONTROL) & 0b11111110); // clear RESET bit  
}

time_t RV8803::getTime(bool blocking) {
 tmElements_t tm;
 getTime(tm, blocking);
 return makeTime(tm);
}

// implementing the time reading as described in the
// applcation notes, section 4.12
void RV8803::getTime(tmElements_t &tm, bool blocking) {
  tmElements_t tm1;
  RTC::getTime(tm, blocking);
  if (blocking) return;   // if we used a blocking call to getTime, then we always wait til the next second starts
  if (tm.Second == 59) {  // be careful when we read 59 seconds because there could have been an increment
    RTC::getTime(tm1, blocking);  // query again
    if (tm1.Second == 59) return; // if again 59, the first reading was OK
    tm = tm1; // otherwise the second reading must be OK
  }
}
  
void RV8803::setAlarm(byte minute, byte hour) {
  setRegister(RV8803_ALARM, bin2bcd(minute)); // set minute alarm
  setRegister(RV8803_ALARM+1, bin2bcd(hour)); // set hour alarm
  setRegister(RV8803_ALARM+2, 0x80); // set weekday alarm to always
  setRegister(RV8803_ALARM+3, 0x80); // set day alarm to always
}

void RV8803::setAlarm(byte minute) {
  setRegister(RV8803_ALARM, bin2bcd(minute)); // set minute alarm
  setRegister(RV8803_ALARM+1, 0x80); // set hour alarm to always
  setRegister(RV8803_ALARM+2, 0x80); // set weekday alarm to always
  setRegister(RV8803_ALARM+3, 0x80); // set day alarm to always
}


void RV8803::enableAlarm(void) { 
  byte ctr = getRegister(RV8803_CONTROL);
  setRegister(RV8803_CONTROL, ctr | 0b1000); // set the AIE bit
}

void RV8803::disableAlarm(void) {
  byte ctr = getRegister(RV8803_CONTROL);
  setRegister(RV8803_CONTROL, (ctr & 0b11110111));  // clear AIE bit
}

bool RV8803::senseAlarm(void) {
  return ((getRegister(RV8803_STATUS) & 0b1000) != 0);
}

void RV8803::clearAlarm(void) {
  byte ctr = getRegister(RV8803_STATUS);
  setRegister(RV8803_STATUS, (ctr & 0b11110111)); 
}


void RV8803::enable32kHz(void) {
  byte clkout = getRegister(RV8803_CLKOUT);
  setRegister(RV8803_CLKOUT, (clkout | 0b1100));
}

void RV8803::enable1Hz(void) {
  byte clkout = getRegister(RV8803_CLKOUT);
  setRegister(RV8803_CLKOUT, (clkout & 0b11110011) | 0b00001000); 
}

// negative values make the clock faster by 0.2384 ppm/LSB
// The range of the internal parameter goes from -32 to +31.
// This means that possible values for offset range from -768 to +744 corresponding to -7.68 ppm to 7.44 ppm 
void RV8803::setOffset(int offset, byte mode) {
  if (mode != 2) {
    if (offset < 0) offset = offset - 12;
    else offset = offset + 12;
    offset = offset/24;
    if (offset < -32) offset = -32;
    else if (offset > 31) offset = 31;
  }
  //Serial.println(offset);
  //Serial.println(offset&0x3F);
  setRegister(RV8803_OFFSET, (offset&0x3F));
}

  
unsigned int RV8803::getOffset(void) {
  return (getRegister(RV8803_OFFSET) & 0x3F);
}
