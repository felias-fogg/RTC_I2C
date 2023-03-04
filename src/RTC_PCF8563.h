/* This is the class for PCF8563 that can be used as part of the RTC_I2C library */

#ifndef _RTC_PCF8563_H_
#define _RTC_PCF8563_H_

#include <RTC_I2C.h>


#define PCF8563_ADDRESS 0x51 // I2C address for PCF8563
#define PCF8563_CLOCKREG 0x02 // Clock register
#define PCF8563_CONTROL 0x00 // Control register
#define PCF8563_CLKOUT  0x0D // Clock out control (and timer)
#define PCF8563_WDAYBASE 0    // wday range from 0 to 6
#define PCF8563_WDAYFIRST false    // wday comes after day of month in clock reg
#define PCF8563_BIT7 0
#define PCF8563_CAP  (RTC_CAP_32KHZ|RTC_CAP_1HZ|RTC_CAP_ALARM)


class PCF8563: public PCFAlarm {
 public:
  PCF8563(void)  {
    _i2caddr = PCF8563_ADDRESS;
    _clockreg = PCF8563_CLOCKREG;
    _wdaybase = PCF8563_WDAYBASE;
    _wdayfirst = PCF8563_WDAYFIRST;
    _capabilities = PCF8563_CAP;
    _bit7set = PCF8563_BIT7;
  };
  void init(byte mode=1);
  bool isValid(void);
  void enableAlarm(void);
  void disableAlarm(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
};
#endif
