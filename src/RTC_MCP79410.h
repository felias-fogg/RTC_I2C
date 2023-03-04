/* This is the class for MCP79410 that can be used as part of the RTC_I2C library */

#ifndef _RTC_MCP79410_H_
#define _RTC_MCP79410_H_

#include <RTC_I2C.h>
#include <util/delay.h>


#define MCP79410_ADDRESS 0x6F // I2C address for MCP79410
#define MCP79410_CLOCKREG 0x00 // Clock register (that is where the seconds start)
#define MCP79410_ALARM   0x0A // Alarm seconds register
#define MCP79410_STATUS 0x03 // Status register
#define MCP79410_CONTROL 0x07 // Control register
#define MCP79410_OFFSET  0x08 // Offset register
#define MCP79410_WDAYBASE 1    // wday range from 1 to 7, 
#define MCP79410_WDAYFIRST true    // wday comes after day of month in clock reg
#define MCP79410_BIT7  1           // start bit for oscillator
#define MCP79410_CAP  (RTC_CAP_32KHZ|RTC_CAP_1HZ|RTC_CAP_ALARM|RTC_CAP_OFFSET)


class MCP79410: public RTC {
 public:
  MCP79410(void) {
  _i2caddr = MCP79410_ADDRESS;
  _clockreg = MCP79410_CLOCKREG;
  _wdaybase = MCP79410_WDAYBASE;
  _wdayfirst = MCP79410_WDAYFIRST;
  _capabilities = MCP79410_CAP;
  _bit7set = MCP79410_BIT7;
};
  void init(byte mode=1);
  bool isValid(void);
  void setAlarm(byte minute, byte hour); // here we can only set the alarm til next match
  bool senseAlarm(void);
  void clearAlarm(void);
  void enableAlarm(void);
  void disableAlarm(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
  void setOffset(int offset, byte mode=1);
};
#endif
