#include "I2CRTC.h"

// try to establish connecttion to RTC after RTC specific parameters have been set
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

bool I2CRTC::setTime(time_t t) {
  tmElements_t tm;

  breakTime(t, tm);
  return setTime(tm);
}

bool I2CRTC::setTime(tmElements_t tm) {
  _wire->beginTransmission(_i2caddr);
  _wire->write(_clockreg);
  _wire->write(bin2bcd(tm.Second)); 
  _wire->write(bin2bcd(tm.Minute));
  _wire->write(bin2bcd(tm.Hour));
  _wire->write(bin2bcd(tm.Wday-1+_wdaybase));
  _wire->write(bin2bcd(tm.Day));
  _wire->write(bin2bcd(tm.Month));
  _wire->write(bin2bcd(tm.Year-30)); // readjust to 2000 instead of 1970!
  if (_wire->endTransmission() == 0) return true;
  return false;
}

time_t I2CRTC::getTime(void) {
  tmElements_t tm;

  getTime(tm);
  return makeTime(tm);
}

void I2CRTC::getTime(tmElements_t &tm) {

  tm = tmElements_t{0,0,0,0,0,0,0};
  _wire->beginTransmission(_i2caddr);
  _wire->write(_clockreg);
  if (_wire->endTransmission() != 0) return;
  if (_wire->requestFrom(_i2caddr, (byte)7) != 7) return;
  tm.Second = bcd2bin(_wire->read()) & 0x7F;
  tm.Minute = bcd2bin(_wire->read()) & 0x7F;
  tm.Hour = bcd2bin(_wire->read()) & 0x3F;
  tm.Wday = (bcd2bin(_wire->read()) & 0x07) - _wdaybase + 1;
  tm.Day = bcd2bin(_wire->read()) & 0x3F;
  tm.Month = bcd2bin(_wire->read()) & 0x1F;
  tm.Year =  bcd2bin(_wire->read()) + 30; // rebase to 1970!
}

bool I2CRTC::setRegister(byte reg, byte val) {
  _wire->beginTransmission(_i2caddr);
  _wire->write(reg);
  _wire->write(val);
  if (_wire->endTransmission() == 0) return true;
  return false;
}

byte I2CRTC::getRegister(byte reg) {
  _wire->beginTransmission(_i2caddr);
  _wire->write(reg);
  if (_wire->endTransmission() != 0) return 0xFF;
  if (_wire->requestFrom(_i2caddr, 1) != 1) return 0xFF;
  return _wire->read();
}
