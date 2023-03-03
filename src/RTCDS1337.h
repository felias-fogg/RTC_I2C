/* This is the class for DS1337 that can be used as part of the I2CRTC library */

#ifndef _RTCDS1337_H_
#define _RTCDS1337_H_

#include <I2CRTC.h>


#define DS1337_ADDRESS 0x68 // I2C address for DS1337
#define DS1337_CLOCKREG 0x00 // Clock register
#define DS1337_ALARM1 0x07 // start of alarm 1 register (seconds)
#define DS1337_CONTROL 0x0E // Control register
#define DS1337_STATUS  0x0F // Status register
#define DS1337_WDAYBASE 1    // wday range from 1 to 7
#define DS1337_WDAYFIRST true //  wday comes before day of month in clock reg
#define DS1337_CAPABIL    (RTC_CAPABIL_32KHZ|RTC_CAPABIL_1HZ|RTC_CAPABIL_ALARM)


class RTCDS1337: public RTCDSAlarm {
 public:
  RTCDS1337(void);
  void init(byte mode);
  bool isValid(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
};
#endif
