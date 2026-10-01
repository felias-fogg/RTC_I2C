#include <RTC_RV8803.h>

bool RV8803::init(__attribute__((unused)) BatteryMode mode) {
  bool success = true;
  success &= setRegister(RV8803_CONTROL, 0); // clear control register
  success &= setRegister(RV8803_STATUS, 0);  // clear all flags
  success &= setRegister(RV8803_CLKOUT, 0);  // 32 KHz output by default
  return success;
}

bool RV8803::isValid() {
  byte status;
  return readRegister(RV8803_STATUS, status) && ((status & 0b11) == 0); // both voltage low flags are cleared
}

// set & clear reset bit when setting time/date
bool RV8803::setTime(timestamp_t t) {
  tm timeParts;
  TimeUtils::fillTimeParts(t, 0, epochStart::unix_epoch, timeParts);
  return setTime(timeParts);
}

bool RV8803::setTime(tm timeParts) {
  byte control;
  if (!readRegister(RV8803_CONTROL, control)) return false;
  bool success = setRegister(RV8803_CONTROL, control | 1); // set RESET bit
  if (success) success = writeTimeRegisters(timeParts);
  success &= setRegister(RV8803_CONTROL, control & 0b11111110); // clear RESET bit
  return success;
}

bool RV8803::writeTimeRegisters(tm timeParts) {
  for (byte attempts = 0; attempts < 4; ++attempts) {
    _wire->beginTransmission(_i2caddr);
    _wire->write(RV8803_CLOCKREG);
    _wire->write(bin2bcd(timeParts.tm_sec));
    _wire->write(bin2bcd(timeParts.tm_min));
    _wire->write(bin2bcd(timeParts.tm_hour));
    _wire->write(1 << timeParts.tm_wday);
    _wire->write(bin2bcd(timeParts.tm_mday));
    _wire->write(bin2bcd(timeParts.tm_mon + 1));
    _wire->write(bin2bcd(timeParts.tm_year - 100));
    if (_wire->endTransmission() == 0 && finishWriteAccess(RV8803_CLOCKREG)) return true;
  }
  return false;
}

bool RV8803::setRegister(byte reg, byte val) {
  for (byte attempts = 0; attempts < 4; ++attempts) {
    _wire->beginTransmission(_i2caddr);
    _wire->write(reg);
    _wire->write(val);
    if (_wire->endTransmission() == 0 && finishWriteAccess(reg)) return true;
  }
  return false;
}

bool RV8803::finishWriteAccess(byte reg) {
  byte value;
  return readRegister(reg, value); // finish with a read operation and STOP, as required by affected silicon
}

timestamp_t RV8803::getTime() {
  tm timeParts;
  getTime(timeParts);
  return TimeUtils::tmToEpochTime(timeParts).getTimestamp();
}

// Implement the time-reading procedure described in the application note, section 4.12.
bool RV8803::getTime(tm &timeParts) {
  tm timeParts1;
  if (!RTC_I2C::getTime(timeParts)) return false;
  if (timeParts.tm_sec == 59) { // be careful when we read 59 seconds because there could have been an increment
    if (!RTC_I2C::getTime(timeParts1)) return false;     // query again
    if (timeParts1.tm_sec != 59) timeParts = timeParts1; // otherwise the second reading must be OK
  }
  return true;
}

bool RV8803::setAlarm(byte minute, byte hour) {
  bool success = true;
  success &= setRegister(RV8803_ALARM, bin2bcd(minute));   // set minute alarm
  success &= setRegister(RV8803_ALARM + 1, bin2bcd(hour)); // set hour alarm
  success &= setRegister(RV8803_ALARM + 2, 0x80);          // set weekday alarm to always
  success &= setRegister(RV8803_ALARM + 3, 0x80);          // set day alarm to always
  return success;
}

bool RV8803::setAlarm(byte minute) {
  bool success = true;
  success &= setRegister(RV8803_ALARM, bin2bcd(minute)); // set minute alarm
  success &= setRegister(RV8803_ALARM + 1, 0x80);        // set hour alarm to always
  success &= setRegister(RV8803_ALARM + 2, 0x80);        // set weekday alarm to always
  success &= setRegister(RV8803_ALARM + 3, 0x80);        // set day alarm to always
  return success;
}


bool RV8803::enableAlarm() {
  byte ctr;
  if (!readRegister(RV8803_CONTROL, ctr)) return false;
  return setRegister(RV8803_CONTROL, ctr | 0b1000); // set the AIE bit
}

bool RV8803::disableAlarm() {
  byte ctr;
  if (!readRegister(RV8803_CONTROL, ctr)) return false;
  return setRegister(RV8803_CONTROL, (ctr & 0b11110111)); // clear AIE bit
}

bool RV8803::senseAlarm() {
  byte status;
  return readRegister(RV8803_STATUS, status) && ((status & 0b1000) != 0);
}

bool RV8803::clearAlarm() {
  byte ctr;
  if (!readRegister(RV8803_STATUS, ctr)) return false;
  return setRegister(RV8803_STATUS, (ctr & 0b11110111));
}


bool RV8803::enable32kHz() {
  byte clkout;
  if (!readRegister(RV8803_CLKOUT, clkout)) return false;
  return setRegister(RV8803_CLKOUT, (clkout | 0b1100));
}

bool RV8803::enable1Hz() {
  byte clkout;
  if (!readRegister(RV8803_CLKOUT, clkout)) return false;
  return setRegister(RV8803_CLKOUT, (clkout & 0b11110011) | 0b00001000);
}

bool RV8803::setOffset(int offset, OffsetMode mode) {
  bool success = true;
  if (mode != OffsetMode::RAW_OFFSET) {
    // Force the offset into range
    if (offset < 0)
      offset = offset - 12;
    else
      offset = offset + 12;
    offset = offset / 24;
    if (offset < -32)
      offset = -32;
    else if (offset > 31)
      offset = 31;
  }
  // set the offset register
  success &= setRegister(RV8803_OFFSET, (offset & 0x3F)); // Serial.println(offset);
  // Serial.println(offset&0x3F);
  return success;
}


unsigned int RV8803::getOffset() {
  return (getRegister(RV8803_OFFSET) & 0x3F);
}

String RV8803::getManufacturer() {
  return F("Micro Crystal");
}

String RV8803::getModel() {
  return F("RV8803");
}
