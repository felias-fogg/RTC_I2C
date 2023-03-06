/* This is the class for DS1307 that can be used as part of the RTC_I2C library */

#ifndef _RTC_DS1307_H_
#define _RTC_DS1307_H_

#include <RTC_I2C.h>

#define DS1307_ADDRESS  0x68     // I2C address for DS1307
#define DS1307_CONTROL  0x07     // Control register
#define DS1307_CLOCKREG 0x00     // Clock register
#define DS1307_OFFSET 0xFF       // no offset register
#define DS1307_WDAYBASE 1        // wday range from 1 to 7
#define DS1307_WDAYFIRST true    // wday comes before day of month in clock reg
#define DS1307_BIT7     0        // no 7th bit needs to be set
#define DS1307_CAP               (RTC_CAP_32KHZ|RTC_CAP_1HZ)


class DS1307: public RTC {
 public:
  DS1307(void) {  _i2caddr = DS1307_ADDRESS;
    _clockreg = DS1307_CLOCKREG;
    _wdaybase = DS1307_WDAYBASE;
    _wdayfirst = DS1307_WDAYFIRST;
    _capabilities = DS1307_CAP;
    _bit7set = DS1307_BIT7;
  };
  void init(byte mode=1);
  bool isValid(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
};
#endif
