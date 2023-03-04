#include <RTC_RV3028U.h>

RV3028U::RV3028U(void) {
  _i2caddr = RV3028_ADDRESS;
  _clockreg = 0;
  _wdaybase = 0;
  _wdayfirst = 0;
  _capabilities = RV3028U_CAP;
}

// get Unix time (from a Unix time counter)
time_t RV3028U::getTime(bool blocking) {
  time_t t1=0, t2;
  int timeout = 0;
  do {
    t2 = t1;
    if (blocking) {
      t1 = getRegister(RV3028_UCLOCK);
      while (++timeout && t1 == getRegister(RV3028_UCLOCK)); // wait until next second is reached
    }
    _wire->beginTransmission(_i2caddr);
    _wire->write(RV3028_UCLOCK);
    if (_wire->endTransmission(false) != 0) return 0;
    if (_wire->requestFrom(_i2caddr, (byte)4) != 4) return 0;
    t1 = 0;
    for (byte i=0; i<4; i++) t1 = (t1 >> 8) | (((time_t)_wire->read())<<24);
    if (blocking) return t1;
  } while (t1 != t2);
  return t1;
}

void RV3028U::getTime(tmElements_t &tm, bool blocking) {
  breakTime(getTime(blocking), tm);
}

void RV3028U::setTime(time_t t) {
  Serial.println(t,HEX);
  setRegister(RV3028_CONTROL+1, getRegister(RV3028_CONTROL+1) | 0b1); // reset counter chain in clock
  _wire->beginTransmission(_i2caddr);
  _wire->write(RV3028_UCLOCK);
  for (byte i=0; i < 4; i++) {
    _wire->write(t & 0xFF);
    t = t >> 8;
  }
  _wire->endTransmission();
}

void RV3028U::setTime(tmElements_t tm) {
  setTime(makeTime(tm));
}

  
