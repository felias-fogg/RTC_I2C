#include "I2CRTC.h"

// try to establish connection to RTC after RTC specific parameters have been set
bool I2CRTC::begin(TwoWire *wi) { 
  if (_i2caddr == 0 || wi == NULL) return false;
  if (_started) return true;
  _wire = wi;
  _wire->begin();
  _wire->beginTransmission(_i2caddr);
  if (_wire->endTransmission() != 0)
    return false;
  return true;
  _started = true;
}

// set time from Unix time
void I2CRTC::setTime(time_t t) {
  tmElements_t tm;
  breakTime(t, tm);
  setTime(tm);
}

// set time from a time record
void I2CRTC::setTime(tmElements_t tm) {
  _wire->beginTransmission(_i2caddr);
  _wire->write(_clockreg);
  _wire->write(bin2bcd(tm.Second)); 
  _wire->write(bin2bcd(tm.Minute));
  _wire->write(bin2bcd(tm.Hour));
  if (_wdayfirst)
    _wire->write(bin2bcd(tm.Wday-1+_wdaybase));
  _wire->write(bin2bcd(tm.Day));
  if (!_wdayfirst)
    _wire->write(bin2bcd(tm.Wday-1+_wdaybase));
  _wire->write(bin2bcd(tm.Month));
  _wire->write(bin2bcd(tm.Year-30)); // readjust to 2000 instead of 1970!
  _wire->endTransmission();
}

// get Unix time
time_t I2CRTC::getTime(void) {
  tmElements_t tm;
  getTime(tm);
  return makeTime(tm);
}

// get time as time record
void I2CRTC::getTime(tmElements_t &tm) {
  tm = tmElements_t{0,0,0,0,0,0,0};
  _wire->beginTransmission(_i2caddr);
  _wire->write(_clockreg);
  if (_wire->endTransmission(false) != 0) return;
  if (_wire->requestFrom(_i2caddr, (byte)7) != 7) return;
  tm.Second = bcd2bin(_wire->read()) & 0x7F;
  tm.Minute = bcd2bin(_wire->read()) & 0x7F;
  tm.Hour = bcd2bin(_wire->read()) & 0x3F;
  if (_wdayfirst) 
    tm.Wday = (bcd2bin(_wire->read()) & 0x07) - _wdaybase + 1;
  tm.Day = bcd2bin(_wire->read()) & 0x3F;
  if (!_wdayfirst) 
    tm.Wday = (bcd2bin(_wire->read()) & 0x07) - _wdaybase + 1;
  tm.Month = bcd2bin(_wire->read()) & 0x1F;
  tm.Year =  bcd2bin(_wire->read()) + 30; // rebase to 1970!
}

// set one RTC register
void I2CRTC::setRegister(byte reg, byte val) {
  //Serial.print(F("setReg(0x")); Serial.print(reg,HEX); Serial.print(F(")=0b")); Serial.println(val,BIN);
  _wire->beginTransmission(_i2caddr);
  _wire->write(reg);
  _wire->write(val);
  _wire->endTransmission();
  //Serial.println(F("Verify:")); Serial.println(getRegister(reg),BIN);
}

// get one RTC register
byte I2CRTC::getRegister(byte reg) {
  byte res;
  //Serial.print(F("getReg(0x")); Serial.print(reg,HEX); Serial.print(F(")=0b"));
  _wire->beginTransmission(_i2caddr);
  _wire->write(reg);
  if (_wire->endTransmission(false) != 0) return 0xFF;
  if (_wire->requestFrom(_i2caddr, (byte)1) != 1) return 0xFF;
  res= _wire->read();
  //Serial.println(res,BIN);
  return res;
}

// Alarm functions for all the Analog Devices RTCs with DS prefix

// Common registers
#define DSALARM_ALARM1  0x07 // start of alarm 1 register (seconds)
#define DSALARM_CONTROL 0x0E // Control register
#define DSALARM_STATUS  0x0F // Status register

void RTCDSAlarm::setAlarm(byte minute, byte hour) {
  setRegister(DSALARM_ALARM1, 0x00); // clear seconds alarm
  setRegister(DSALARM_ALARM1+1, bin2bcd(minute)); // set minute alarm
  setRegister(DSALARM_ALARM1+2, bin2bcd(hour)); // set hour alarm
  setRegister(DSALARM_ALARM1+3, 0x80); // set day alarm to always
}

void RTCDSAlarm::enableAlarm(void) {
  byte ctr = getRegister(DSALARM_CONTROL);
  setRegister(DSALARM_CONTROL, (ctr & 0b11111110) | 0b00000001);
}

void RTCDSAlarm::disableAlarm(void) {
  byte ctr = getRegister(DSALARM_CONTROL);
  setRegister(DSALARM_CONTROL, (ctr & 0b11111110) | 0b00000000); 
}

bool RTCDSAlarm::senseAlarm(void) {
  return getRegister(DSALARM_STATUS) & 0x01;
}

void RTCDSAlarm::clearAlarm(void) {
  byte ctr = getRegister(DSALARM_STATUS);
  setRegister(DSALARM_STATUS, (ctr & 0b11111110) | 0b00000000); 
}


// Alarm functions for all the NXP RTCs with PCF prefix

// Common registers
#define PCFALARM_STATUS 0x00 // Control register

void RTCPCFAlarm::setAlarm(byte minute, byte hour) {
  setRegister(_clockreg+7, bin2bcd(minute)); // set minute alarm
  setRegister(_clockreg+8, bin2bcd(hour)); // set hour alarm
  setRegister(_clockreg+9, 0x80); // set day alarm to always
  setRegister(_clockreg+10, 0x80); // set weekday alarm to always
}

bool RTCPCFAlarm::senseAlarm(void) {
  return ((getRegister(PCFALARM_STATUS) & 0b1000) != 0);
}

void RTCPCFAlarm::clearAlarm(void) {
  byte ctr = getRegister(PCFALARM_STATUS);
  setRegister(PCFALARM_STATUS, (ctr & 0b11110111) | 0b00000000); 
}
