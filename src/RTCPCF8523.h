/* This is the class for PCF8523 that can be used as part of the I2CRTC library */

#ifndef _RTCPCF8523_H_
#define _RTCPCF8523_H_

#include <I2CRTC.h>


#define PCF8523_ADDRESS 0x68 // I2C address for PCF8523
#define PCF8523_CLOCKREG 0x03 // Clock register
#define PCF8523_CONTROL 0x00 // Control register
#define PCF8523_OFFSET  0x0E // Offset register
#define PCF8523_CLKOUT  0x0F // Clock out control (and timer)
#define PCF8523_WDAYBASE 0    // wday range from 0 to 6
#define PCF8523_WDAYFIRST false    // wday comes after day of month in clock reg
#define PCF8523_CAPABIL  (RTC_CAPABIL_32KHZ|RTC_CAPABIL_1HZ|RTC_CAPABIL_ALARM|RTC_CAPABIL_OFFSET)


class RTCPCF8523: public RTCPCFAlarm {
 public:
  RTCPCF8523(void);
  void init(void);
  bool isValid(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
  void setOffset(int offset, byte mode=1);
};
#endif
