/*****************************************************************************************
 *
 * This library provides a universal interface to many different RTCs making it possible
 * to use these RTCs in parallel, not caring about their particular features.
 *
 * License: MIT
 *
 *
 *****************************************************************************************/

#ifndef _UNIVRTC_H_
#define _UNIVRTC_H_

#include <Arduino.h>
#include <TimeLib.h>
#include <Wire.h>

#define TIMEBYTES 7

/* A generic RTC base class */
class I2CRTC {
 public:
  bool begin(TwoWire *wi = &Wire);
  virtual void init(void) = 0;
  virtual bool isValid(void) = 0;
  virtual bool setTime(time_t t);
  virtual bool setTime(tmElements_t tm);
  virtual time_t getTime(void);
  virtual void getTime(tmElements_t &tm);
  virtual bool setRegister(byte reg, byte val);
  virtual byte getRegister(byte reg);

 protected:
  static byte bcd2bin(byte val) { return val - 6 * (val >> 4); }
  static byte bin2bcd(byte val) { return val + 6 * (val / 10); }
  byte _i2caddr = 0;
  TwoWire *_wire = NULL;
  bool _started = false;
  byte _clockreg;
  byte _wdaybase = 1;
};

#endif
