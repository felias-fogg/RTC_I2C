/* This is the class for PCF8563 that can be used as part of the I2CRTC library */

#ifndef _RTCPCF8563_H_
#define _RTCPCF8563_H_

#include <I2CRTC.h>


#define PCF8563_ADDRESS 0x51 // I2C address for PCF8563
#define PCF8563_CLOCKREG 0x02 // Clock register
#define PCF8563_CONTROL 0x00 // Control register
#define PCF8563_CLKOUT  0x0D // Clock out control (and timer)
#define PCF8563_WDAYBASE 0    // wday range from 0 to 6
#define PCF8563_WDAYFIRST false    // wday comes after day of month in clock reg
#define PCF8563_CAPABIL  (RTC_CAPABIL_32KHZ|RTC_CAPABIL_1HZ|RTC_CAPABIL_ALARM)


class RTCPCF8563: public RTCPCFAlarm {
 public:
  RTCPCF8563(void);
  void init(byte mode);
  bool isValid(void);
  void enableAlarm(void);
  void disableAlarm(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
};
#endif
