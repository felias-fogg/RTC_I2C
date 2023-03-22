/* This is the class for DS3231 that can be used as part of the RTC_I2C library */

#ifndef _RTC_DS3231_H_
#define _RTC_DS3231_H_

#include <RTC_I2C.h>


#define DS3231_ADDRESS 0x68 // I2C address for DS3231
#define DS3231_CLOCKREG 0x00 // Clock register
#define DS3231_ALARM1  0x07 // start of alarm 1 register (seconds)
#define DS3231_CONTROL 0x0E // Control register
#define DS3231_STATUS  0x0F // Status register
#define DS3231_OFFSET  0x10 // Offset register
#define DS3231_TEMPMSB 0x11 // MSB of temperature register
#define DS3231_WDAYBASE 1    // wday range from 1 to 7
#define DS3231_WDAYFIRST true    // wday comes before day of month in clock reg
#define DS3231_BIT7  0
#define DS3231_CAP  (RTC_CAP_32KHZ|RTC_CAP_1HZ|RTC_CAP_ALARM|RTC_CAP_HOURLY_ALARM|RTC_CAP_OFFSET|RTC_CAP_TEMP)


class DS3231: public DSAlarm {
 public:
  DS3231(void) {
  _i2caddr = DS3231_ADDRESS;
  _clockreg = DS3231_CLOCKREG;
  _wdaybase = DS3231_WDAYBASE;
  _wdayfirst = DS3231_WDAYFIRST;
  _capabilities = DS3231_CAP;
  _bit7set = DS3231_BIT7;
  };
  void init(byte mode=1);
  bool isValid(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
  int getTemp(void);
  void setOffset(int offset, byte mode=1);
  unsigned int getOffset(void);
};
#endif
