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
#define RTC_CAPABIL_TEMP 0x10
#define RTC_CAPABIL_OFFSET 0x08
#define RTC_CAPABIL_ALARM 0x04
#define RTC_CAPABIL_1HZ 0x02
#define RTC_CAPABIL_32KHZ 0x01


/* A generic RTC base class */
class I2CRTC {
 public:
  bool begin(TwoWire *wi = &Wire);
  virtual void init(void) = 0;
  virtual bool isValid(void) = 0;
  virtual void setTime(time_t t);
  virtual void setTime(tmElements_t tm);
  virtual time_t getTime(void);
  virtual void getTime(tmElements_t &tm);
  virtual void enable32kHz(void) { };
  virtual void disable32kHz(void) { };
  virtual void enable1Hz(void) { };
  virtual void disable1Hz(void) { };
  virtual void setAlarm(__attribute__ ((unused)) byte minute, __attribute__ ((unused)) byte hour) {  };
  virtual void enableAlarm(void) { };
  virtual void disableAlarm(void) { };
  virtual bool senseAlarm(void) { return false; };
  virtual void clearAlarm(void) { };
  virtual void setOffset(__attribute__ ((unused)) int offset, __attribute__ ((unused)) byte mode = 1) {  };
  virtual int  getTemp(void) { return -128; };
  virtual void setRegister(byte reg, byte val);
  virtual byte getRegister(byte reg);
  virtual byte getCapabilities(void) { return _capabilities; };

 protected:
  static byte bcd2bin(byte val) { return val - 6 * (val >> 4); }
  static byte bin2bcd(byte val) { return val + 6 * (val / 10); }
  byte _i2caddr = 0; 
  TwoWire *_wire = NULL;
  bool _started = false;
  byte _clockreg;  // where clock reg starts
  byte _wdaybase;  // base of weekday counting 
  bool _wdayfirst; // true when weekday comes before day in clock register
  byte _capabilities; // lists all capabilities of this RTC
};

class RTCDSAlarm : public I2CRTC {
 public:
  void setAlarm(byte minute, byte hour);
  void enableAlarm(void);
  void disableAlarm(void);
  bool senseAlarm(void);
  void clearAlarm(void);
};

class RTCPCFAlarm : public I2CRTC {
 public:
  void setAlarm(byte minute, byte hour);
  bool senseAlarm(void);
  void clearAlarm(void);
};
#endif
