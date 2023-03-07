#include "RTC_I2C.h"


// try to establish connection to RTC after RTC specific parameters have been set
bool RTC::begin(TwoWire *wi) { 
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
void RTC::setTime(time_t t) {
  tmElements_t tm;
  breakTime(t, tm);
  setTime(tm);
}

// set time from a time record
void RTC::setTime(tmElements_t tm) {
  _wire->beginTransmission(_i2caddr);
  _wire->write((_capabilities&RTC_CAP_SREGADDR) ? (_clockreg<<4) : _clockreg);
  _wire->write(bin2bcd(tm.Second) | ((_bit7set & 1) ? 0x80 : 0)); 
  _wire->write(bin2bcd(tm.Minute) | ((_bit7set & (1<<1)) ? 0x80 : 0));
  _wire->write(bin2bcd(tm.Hour)   | ((_bit7set & (1<<2)) ? 0x80 : 0));
  if (!_wdayfirst)
    _wire->write(bin2bcd(tm.Day)   | ((_bit7set & (1<<4)) ? 0x80 : 0));
  _wire->write((_wdaybase < 2 ? bin2bcd(tm.Wday-1+_wdaybase) : (1<<(tm.Wday-1))) | ((_bit7set & (1<<3)) ? 0x80 : 0));
  if (_wdayfirst)
    _wire->write(bin2bcd(tm.Day) | ((_bit7set & (1<<4)) ? 0x80 : 0));
  _wire->write(bin2bcd(tm.Month) | ((_bit7set & (1<<5)) ? 0x80 : 0));
  _wire->write(bin2bcd(tm.Year-30)); // readjust to 2000 instead of 1970!
  _wire->endTransmission();
}

// get Unix time
time_t RTC::getTime(bool blocking) {
  tmElements_t tm;
  getTime(tm, blocking);
  return makeTime(tm);
}

// get time as time record
void RTC::getTime(tmElements_t &tm, bool blocking) {
  int timeout = 0;
  byte sec;
  tm = tmElements_t{0,0,0,0,0,0,0};

  if (blocking) {
    sec = getRegister(_clockreg);
    while (++timeout && sec ==
	   getRegister((_capabilities&RTC_CAP_SREGADDR) ?
		       (_clockreg<<4) : _clockreg));// wait until next second is reached 
  }  
  _wire->beginTransmission(_i2caddr);
  _wire->write((_capabilities&RTC_CAP_SREGADDR) ? (_clockreg<<4) : _clockreg);
  if (_wire->endTransmission(false) != 0) return;
  if (_wire->requestFrom(_i2caddr, (byte)7) != 7) return;
  tm.Second = bcd2bin(_wire->read() & 0x7F);
  tm.Minute = bcd2bin(_wire->read() & 0x7F);
  tm.Hour = bcd2bin(_wire->read() & 0x3F) ;
  if (!_wdayfirst) 
    tm.Day = bcd2bin(_wire->read() & 0x3F);
  tm.Wday = ( _wdaybase < 2 ? (bcd2bin(_wire->read() & 0x07)) - _wdaybase + 1 : decodewday(_wire->read()));
  if (_wdayfirst) 
    tm.Day = bcd2bin(_wire->read() & 0x3F);
  tm.Month = bcd2bin(_wire->read() & 0x1F) ;
  tm.Year =  bcd2bin(_wire->read()) + 30; // rebase to 1970!
}

byte RTC::decodewday(byte bits) {
  for (byte res = 1; res < 8; res++) {
    if (bits & 1) return res;
    bits = bits >> 1;
  }
  return 1; // default value
}
    
// set one RTC register
void RTC::setRegister(byte reg, byte val) {
  //Serial.print(F("setReg(0x")); Serial.print(reg,HEX); Serial.print(F(")=0b")); Serial.println(val,BIN);
  _wire->beginTransmission(_i2caddr);
  _wire->write((_capabilities&RTC_CAP_SREGADDR) ? (reg<<4) : reg);
  _wire->write(val);
  _wire->endTransmission();
  //Serial.println(F("Verify:")); Serial.println(getRegister(reg),BIN);
}

// get one RTC register
byte RTC::getRegister(byte reg) {
  byte res;
  //Serial.print(F("getReg(0x")); Serial.print(reg,HEX); Serial.print(F(")=0b"));
  _wire->beginTransmission(_i2caddr);
  _wire->write((_capabilities&RTC_CAP_SREGADDR) ? (reg<<4) : reg);
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

void DSAlarm::setAlarm(byte minute, byte hour) {
  setRegister(DSALARM_ALARM1, 0x00); // clear seconds alarm
  setRegister(DSALARM_ALARM1+1, bin2bcd(minute)); // set minute alarm
  setRegister(DSALARM_ALARM1+2, bin2bcd(hour)); // set hour alarm
  setRegister(DSALARM_ALARM1+3, 0x80); // set day alarm to always
}

void DSAlarm::enableAlarm(void) {
  byte ctr = getRegister(DSALARM_CONTROL);
  setRegister(DSALARM_CONTROL, (ctr & 0b11111110) | 0b00000001);
}

void DSAlarm::disableAlarm(void) {
  byte ctr = getRegister(DSALARM_CONTROL);
  setRegister(DSALARM_CONTROL, (ctr & 0b11111110) | 0b00000000); 
}

bool DSAlarm::senseAlarm(void) {
  return getRegister(DSALARM_STATUS) & 0x01;
}

void DSAlarm::clearAlarm(void) {
  byte ctr = getRegister(DSALARM_STATUS);
  setRegister(DSALARM_STATUS, (ctr & 0b11111110) | 0b00000000); 
}


// Alarm functions for all the NXP RTCs with PCF prefix

// Common registers
#define PCFALARM_STATUS 0x01 // Control register

void PCFAlarm::setAlarm(byte minute, byte hour) {
  setRegister(_clockreg+7, bin2bcd(minute)); // set minute alarm
  setRegister(_clockreg+8, bin2bcd(hour)); // set hour alarm
  setRegister(_clockreg+9, 0x80); // set day alarm to always
  setRegister(_clockreg+10, 0x80); // set weekday alarm to always
}

bool PCFAlarm::senseAlarm(void) {
  return ((getRegister(PCFALARM_STATUS) & 0b1000) != 0);
}

void PCFAlarm::clearAlarm(void) {
  byte ctr = getRegister(PCFALARM_STATUS);
  setRegister(PCFALARM_STATUS, (ctr & 0b11110111) | 0b00000000); 
}
