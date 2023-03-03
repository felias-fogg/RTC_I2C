#include <RTCRV8803.h>

RTCRV8803::RTCRV8803(void) {
  _i2caddr = RV8803_ADDRESS;
  _clockreg = RV8803_CLOCKREG;
  _wdaybase = RV8803_WDAYBASE;
  _wdayfirst = RV8803_WDAYFIRST;
  _capabilities = RV8803_CAPABIL;
}

void RTCRV8803::init(void) {
  setRegister(RV8803_CONTROL, 0); // clear control register
  setRegister(RV8803_STATUS, 0);  // clear all flags
  setRegister(RV8803_CLKOUT, 0);  // 32 KHz output by default
}

bool RTCRV8803::isValid(void) {
  return ((getRegister(RV8803_STATUS) & 0b11) == 0); // both voltage low flags are cleared
}

// set & clear reset bit when setting time/date
void RTCRV8803::setTime(time_t t) {
  setRegister(RV8803_CONTROL, getRegister(RV8803_CONTROL) | 1); // set RESET bit
  I2CRTC::setTime(t);
  setRegister(RV8803_CONTROL, getRegister(RV8803_CONTROL) & 0b11111110); // clear RESET bit  
}

void RTCRV8803::setTime(tmElements_t tm) {
  setRegister(RV8803_CONTROL, getRegister(RV8803_CONTROL) | 1); // set RESET bit
  I2CRTC::setTime(tm);
  setRegister(RV8803_CONTROL, getRegister(RV8803_CONTROL) & 0b11111110); // clear RESET bit  
}

time_t RTCRV8803::getTime(bool blocking) {
 tmElements_t tm;
 getTime(tm, blocking);
 return makeTime(tm);
}

// implementing the time reading as described in the
// applcation notes, section 4.12
void RTCRV8803::getTime(tmElements_t &tm, bool blocking) {
  tmElements_t tm1;
  I2CRTC::getTime(tm, blocking);
  if (blocking) return;   // if we used a blocking call to getTime, then we always wait til the next second starts
  if (tm.Second == 59) {  // be careful when we read 59 seconds because there could have been an increment
    I2CRTC::getTime(tm1, blocking);  // query again
    if (tm1.Second == 59) return; // if again 59, the first reading was OK
    tm = tm1; // otherwise the second reading must be OK
  }
}
  
void RTCRV8803::setAlarm(byte minute, byte hour) {
  setRegister(RV8803_ALARM, bin2bcd(minute)); // set minute alarm
  setRegister(RV8803_ALARM+1, bin2bcd(hour)); // set hour alarm
  setRegister(RV8803_ALARM+2, 0x80); // set weekday alarm to always
  setRegister(RV8803_ALARM+3, 0x80); // set day alarm to always
}


void RTCRV8803::enableAlarm(void) { 
  byte ctr = getRegister(RV8803_CONTROL);
  setRegister(RV8803_CONTROL, ctr | 0b1000); // set the AIE bit
}

void RTCRV8803::disableAlarm(void) {
  byte ctr = getRegister(RV8803_CONTROL);
  setRegister(RV8803_CONTROL, (ctr & 0b11110111));  // clear AIE bit
}

bool RTCRV8803::senseAlarm(void) {
  return ((getRegister(RV8803_STATUS) & 0b1000) != 0);
}

void RTCRV8803::clearAlarm(void) {
  byte ctr = getRegister(RV8803_STATUS);
  setRegister(RV8803_STATUS, (ctr & 0b11110111)); 
}


void RTCRV8803::enable32kHz(void) {
  byte clkout = getRegister(RV8803_CLKOUT);
  setRegister(RV8803_CLKOUT, (clkout | 0b1100));
}

void RTCRV8803::enable1Hz(void) {
  byte clkout = getRegister(RV8803_CLKOUT);
  setRegister(RV8803_CLKOUT, (clkout & 0b11110011) | 0b00001000); 
}

// negative values make the clock faster by 0.2384 ppm/LSB
// The range of the internal parameter goes from -32 to +31.
// This means that possible values for offset range from -768 to +744 corresponding to -7.68 ppm to 7.44 ppm 
void RTCRV8803::setOffset(int offset, byte mode) {
  if (offset < 0) offset = offset - 12;
  else offset = offset + 12;
  offset = offset/24;
  if (offset < -32) offset = -32;
  else if (offset > 31) offset = 31;
  Serial.println(offset);
  Serial.println(offset&0x3F);
  setRegister(RV8803_OFFSET, (offset&0x3F));
}

  
