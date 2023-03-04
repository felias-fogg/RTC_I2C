/* This is the class for DS1337 that can be used as part of the RTC_I2C library */

#ifndef _RTC_DS1337_H_
#define _RTC_DS1337_H_

#include <RTC_I2C.h>


#define DS1337_ADDRESS 0x68 // I2C address for DS1337
#define DS1337_CLOCKREG 0x00 // Clock register
#define DS1337_ALARM1 0x07 // start of alarm 1 register (seconds)
#define DS1337_CONTROL 0x0E // Control register
#define DS1337_STATUS  0x0F // Status register
#define DS1337_WDAYBASE 1    // wday range from 1 to 7
#define DS1337_WDAYFIRST true //  wday comes before day of month in clock reg
#define DS1337_BIT7   0
#define DS1337_CAP    (RTC_CAP_32KHZ|RTC_CAP_1HZ|RTC_CAP_ALARM)


class DS1337: public DSAlarm {
 public:
  DS1337(void)  {
  _i2caddr = DS1337_ADDRESS;
  _clockreg = DS1337_CLOCKREG;
  _wdaybase = DS1337_WDAYBASE;
  _wdayfirst = DS1337_WDAYFIRST;
  _capabilities = DS1337_CAP;
  _bit7set = DS1337_BIT7;
  };
  void init(byte mode=1);
  bool isValid(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
};
#endif
