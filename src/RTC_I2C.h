/*****************************************************************************************
 *
 * This library provides a universal interface to many different RTCs making it possible
 * to use these RTCs in parallel, not caring about their particular features.
 *
 * License: MIT
 *
 *
 *****************************************************************************************/

#ifndef _RTC_I2C_H_
#define _RTC_I2C_H_

#include <Arduino.h>
#include <TimeLib.h>
#include <Wire.h>

#define TIMEBYTES 7
#define RTC_CAP_32KHZ 0x01       // can generate 32 kHz signal 
#define RTC_CAP_1HZ 0x02         // can generate 1 Hz signal
#define RTC_CAP_ALARM 0x04       // has alarm functionality to set hour and minute
#define RTC_CAP_HOURLY_ALARM 0x08// can raise an alarm every hour
#define RTC_CAP_OFFSET 0x10      // has an offset register
#define RTC_CAP_TEMP 0x20        // has a temperature sensor
#define RTC_CAP_SREGADDR 0x40    // uses a strange format for register addresses (upper nibble)


/* A generic RTC base class */
class RTC {
 public:
  bool begin(TwoWire *wi = &Wire);
  virtual void init(byte mode=1) = 0;
  virtual bool isValid(void) = 0;
  virtual void setTime(time_t t);
  virtual void setTime(tmElements_t tm);
  virtual time_t getTime(bool blocking = false);
  virtual void getTime(tmElements_t &tm, bool blocking = false);
  virtual void enable32kHz(void) { };
  virtual void disable32kHz(void) { };
  virtual void enable1Hz(void) { };
  virtual void disable1Hz(void) { };
  virtual void setAlarm(__attribute__ ((unused)) byte minute, __attribute__ ((unused)) byte hour) {  };
  virtual void setAlarm(__attribute__ ((unused)) byte minute) {  };
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
  static byte decodewday(byte bits);
  byte _i2caddr = 0; 
  TwoWire *_wire = NULL;
  bool _started = false;
  byte _clockreg;  // where clock reg starts
  byte _wdaybase;  // base of weekday counting 
  bool _wdayfirst; // true when weekday comes before day in clock register
  byte _capabilities; // lists all capabilities of this RTC
  byte _bit7set; // if the 7th bit in a byte of the clock register must be set (bit 0=sec, bit 1=min, ...)
};

class DSAlarm : public RTC {
 public:
  void setAlarm(byte minute, byte hour);
  void setAlarm(byte minute);
  void enableAlarm(void);
  void disableAlarm(void);
  bool senseAlarm(void);
  void clearAlarm(void);
};

class PCFAlarm : public RTC {
 public:
  void setAlarm(byte minute, byte hour);
  void setAlarm(byte minute);
  bool senseAlarm(void);
  void clearAlarm(void);
};
#endif
