/* This is the class for SD2405 that can be used as part of the RTC_I2C library */

#ifndef _RTC_SD2405_H_
#define _RTC_SD2405_H_

#include <RTC_I2C.h>


#define SD2405_ADDRESS 0x32 // I2C address for SD2405
#define SD2405_CLOCKREG 0x00 // Clock register (that is where the seconds start)
#define SD2405_ALARM   0x07 // Alarm seconds register
#define SD2405_CONTROL 0x0F // Control register
#define SD2405_OFFSET  0x12 // Offset register

#define SD2405_WDAYBASE 0    // wday range from 0 to 6
#define SD2405_WDAYFIRST true  // wday comes before day of month in clock reg
#define SD2405_BIT7 (1<<2)      // The 24H flag!
#define SD2405_CAP  (RTC_CAP_32KHZ|RTC_CAP_1HZ|RTC_CAP_ALARM|RTC_CAP_HOURLY_ALARM|RTC_CAP_OFFSET)


class SD2405: public RTC {
 public:
  SD2405(void) {
    _i2caddr = SD2405_ADDRESS;
    _clockreg = SD2405_CLOCKREG;
    _wdaybase = SD2405_WDAYBASE;
    _wdayfirst = SD2405_WDAYFIRST;
    _capabilities = SD2405_CAP;
    _bit7set = SD2405_BIT7;
  };
  void init(byte mode=1);
  bool isValid(void);
  void setAlarm(byte minute, byte hour); // here we can only set the alarm til next match
  void setAlarm(byte minute); // hourly alarm at a particular minute
  bool senseAlarm(void);
  void clearAlarm(void);
  void enableAlarm(void);
  void disableAlarm(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
  void setOffset(int offset, byte mode=1);
  unsigned int getOffset(void);
};
#endif
