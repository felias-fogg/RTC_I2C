#include "RTC_I2C.h"


// try to establish connection to RTC after RTC specific parameters have been set
bool RTC_I2C::begin(TwoWire *wi, BatteryMode mode) {
  if (_i2caddr == 0 || wi == NULL) return false;
  if (_started) return true;
  _wire = wi;
  _wire->begin();
  _wire->beginTransmission(_i2caddr);
  if (_wire->endTransmission() != 0) return false;
  _started = init(mode);
  return _started;
}

// set time from epochTime object
bool RTC_I2C::setTime(epochTime eTime) {
  return setTime(eTime.getTimestamp());
}

// set time from Unix time
bool RTC_I2C::setTime(timestamp_t t) {
  tm timeParts;
  TimeUtils::fillTimeParts(t, 0, epochStart::unix_epoch, timeParts);
  return setTime(timeParts);
}

// set time from a time record
bool RTC_I2C::setTime(tm timeParts) {
  _wire->beginTransmission(_i2caddr);
  _wire->write((_capabilities & RTC_CAP_SREGADDR) ? (_clockreg << 4) : _clockreg);
  _wire->write(bin2bcd(timeParts.tm_sec) | ((_bit7set & 1) ? 0x80 : 0));         // seconds after the minute
  _wire->write(bin2bcd(timeParts.tm_min) | ((_bit7set & (1 << 1)) ? 0x80 : 0));  // minutes after the hour
  _wire->write(bin2bcd(timeParts.tm_hour) | ((_bit7set & (1 << 2)) ? 0x80 : 0)); // hours since midnight
  if (!_wdayfirst) _wire->write(bin2bcd(timeParts.tm_mday) | ((_bit7set & (1 << 4)) ? 0x80 : 0));
  // ^^ day of the month, if it comes before the day of the week in the RTC register
  _wire->write((_wdaybase < 2 ? bin2bcd(timeParts.tm_wday + _wdaybase) : (1 << (timeParts.tm_wday))) |
               ((_bit7set & (1 << 3)) ? 0x80 : 0));
  // ^^ day of the week, adjusted from tm's 0-6 numbering to whatever the RTC uses as its _wdaybase
  if (_wdayfirst) _wire->write(bin2bcd(timeParts.tm_mday) | ((_bit7set & (1 << 4)) ? 0x80 : 0));
  // ^^ day of the month, if it comes after the day of the week in the RTC register
  _wire->write(bin2bcd(timeParts.tm_mon + 1) | ((_bit7set & (1 << 5)) ? 0x80 : 0));
  // ^^ month of the year, shifted from 0-indexed to 1-indexed
  _wire->write(bin2bcd(timeParts.tm_year - 100));
  // ^^ years since 1900 (as in tm structure) converted to years since 2000 used by supported RTCs
  return _wire->endTransmission() == 0;
}

// get Unix time
timestamp_t RTC_I2C::getTime() {
  tm timeParts;
  getTime(timeParts);
  return TimeUtils::tmToEpochTime(timeParts).getTimestamp();
}

// get time as time record
bool RTC_I2C::getTime(tm &timeParts) {
  timeParts = tm{0, 0, 0, 0, 0, 0, 0, 0, 0};

  _wire->beginTransmission(_i2caddr);
  _wire->write((_capabilities & RTC_CAP_SREGADDR) ? (_clockreg << 4) : _clockreg);
  if (_wire->endTransmission((_capabilities & RTC_CAP_STOP_BEFORE_READ) != 0) != 0) return false;
  if (_wire->requestFrom(_i2caddr, (byte)7) != 7) return false;
  timeParts.tm_sec = bcd2bin(_wire->read() & 0x7F);  // seconds after the minute
  timeParts.tm_min = bcd2bin(_wire->read() & 0x7F);  // minutes after the hour
  timeParts.tm_hour = bcd2bin(_wire->read() & 0x3F); // hours since midnight
  if (!_wdayfirst) timeParts.tm_mday = bcd2bin(_wire->read() & 0x3F);
  // ^^ day of the month, if it comes before the day of the week in the RTC register
  timeParts.tm_wday = (_wdaybase < 2 ? (bcd2bin(_wire->read() & 0x07)) - _wdaybase : decodewday(_wire->read()));
  // ^^ day of the week, adjusted from whatever the RTC uses as its _wdaybase to tm's 0-6 numbering
  if (_wdayfirst) timeParts.tm_mday = bcd2bin(_wire->read() & 0x3F);
  // ^^ day of the month, if it comes after the day of the week in the RTC register
  timeParts.tm_mon = bcd2bin(_wire->read() & 0x1F) - 1;
  // ^^ month of the year, shifted from 1-indexed to 0-indexed
  timeParts.tm_year = bcd2bin(_wire->read()) + 100;
  // ^^ years since 2000 used by supported RTCs converted to years since 1900 (as in tm structure)
  return true;
}

byte RTC_I2C::decodewday(byte bits) {
  for (byte res = 1; res < 8; res++) {
    if (bits & 1) return res - 1;
    bits = bits >> 1;
  }
  return 0; // default value
}

// set one RTC register
bool RTC_I2C::setRegister(byte reg, byte val) {
  // Serial.print(F("setReg(0x")); Serial.print(reg,HEX); Serial.print(F(")=0b")); Serial.println(val,BIN);
  _wire->beginTransmission(_i2caddr);
  _wire->write((_capabilities & RTC_CAP_SREGADDR) ? (reg << 4) : reg);
  _wire->write(val);
  bool success = _wire->endTransmission() == 0;
  // Serial.println(F("Verify:")); Serial.println(getRegister(reg),BIN);
  return success;
}

// get one RTC register
byte RTC_I2C::getRegister(byte reg) {
  byte res = 0xFF;
  readRegister(reg, res);
  return res;
}

bool RTC_I2C::readRegister(byte reg, byte &res) {
  // Serial.print(F("getReg(0x")); Serial.print(reg,HEX); Serial.print(F(")=0b"));
  _wire->beginTransmission(_i2caddr);
  _wire->write((_capabilities & RTC_CAP_SREGADDR) ? (reg << 4) : reg);
  if (_wire->endTransmission((_capabilities & RTC_CAP_STOP_BEFORE_READ) != 0) != 0) return false;
  if (_wire->requestFrom(_i2caddr, (byte)1) != 1) return false;
  res = _wire->read();
  // Serial.println(res,BIN);
  return true;
}

// Alarm functions for all the Analog Devices RTCs with DS prefix

// Common registers
/// Start of Alarm 1 registers for the shared DS-family alarm implementation; Alarm 1 begins at address 0x07.
#define DSALARM_ALARM1 0x07
/// DS-family Control Register; referred to as Control Register in the DS1337/DS3231 documentation (ADDRESS 0x0E).
#define DSALARM_CONTROL 0x0E
/// DS-family Status Register; referred to as Status Register in the DS1337/DS3231 documentation (ADDRESS 0x0F).
#define DSALARM_STATUS 0x0F

bool DSAlarm::setAlarm(byte minute, byte hour) {
  bool success = true;
  success &= setRegister(DSALARM_ALARM1, 0x00);                // clear seconds alarm
  success &= setRegister(DSALARM_ALARM1 + 1, bin2bcd(minute)); // set minute alarm
  success &= setRegister(DSALARM_ALARM1 + 2, bin2bcd(hour));   // set hour alarm
  success &= setRegister(DSALARM_ALARM1 + 3, 0x80);            // set day alarm to always
  return success;
}

bool DSAlarm::setAlarm(byte minute) {
  bool success = true;
  success &= setRegister(DSALARM_ALARM1, 0x00);                // clear seconds alarm
  success &= setRegister(DSALARM_ALARM1 + 1, bin2bcd(minute)); // set minute alarm
  success &= setRegister(DSALARM_ALARM1 + 2, 0x80);            // set hour alarm to always
  success &= setRegister(DSALARM_ALARM1 + 3, 0x80);            // set day alarm to always
  return success;
}

bool DSAlarm::enableAlarm() {
  byte ctr;
  if (!readRegister(DSALARM_CONTROL, ctr)) return false;
  return setRegister(DSALARM_CONTROL, (ctr & 0b11111110) | 0b00000001);
}

bool DSAlarm::disableAlarm() {
  byte ctr;
  if (!readRegister(DSALARM_CONTROL, ctr)) return false;
  return setRegister(DSALARM_CONTROL, (ctr & 0b11111110) | 0b00000000);
}

bool DSAlarm::senseAlarm() {
  byte status;
  return readRegister(DSALARM_STATUS, status) && (status & 0x01);
}

bool DSAlarm::clearAlarm() {
  byte ctr;
  if (!readRegister(DSALARM_STATUS, ctr)) return false;
  return setRegister(DSALARM_STATUS, (ctr & 0b11111110) | 0b00000000);
}


// Alarm functions for all the NXP RTCs with PCF prefix

// Common registers
/// PCF-family Control/status register 2; the shared alarm flag is in the register at address 0x01.
#define PCFALARM_STATUS 0x01

bool PCFAlarm::setAlarm(byte minute, byte hour) {
  bool success = true;
  success &= setRegister(_clockreg + 7, bin2bcd(minute)); // set minute alarm
  success &= setRegister(_clockreg + 8, bin2bcd(hour));   // set hour alarm
  success &= setRegister(_clockreg + 9, 0x80);            // set day alarm to always
  success &= setRegister(_clockreg + 10, 0x80);           // set weekday alarm to always
  return success;
}

bool PCFAlarm::setAlarm(byte minute) {
  bool success = true;
  success &= setRegister(_clockreg + 7, bin2bcd(minute)); // set minute alarm
  success &= setRegister(_clockreg + 8, 0x80);            // set hour alarm to always
  success &= setRegister(_clockreg + 9, 0x80);            // set day alarm to always
  success &= setRegister(_clockreg + 10, 0x80);           // set weekday alarm to always
  return success;
}

bool PCFAlarm::senseAlarm() {
  byte status;
  return readRegister(PCFALARM_STATUS, status) && ((status & 0b1000) != 0);
}

bool PCFAlarm::clearAlarm() {
  byte ctr;
  if (!readRegister(PCFALARM_STATUS, ctr)) return false;
  return setRegister(PCFALARM_STATUS, (ctr & 0b11110111) | 0b00000000);
}

// Manufacturer and model information functions
String RTC_I2C::getMakeModel() {
  return String(getManufacturer()) + " " + String(getModel());
}
