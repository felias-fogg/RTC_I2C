/* This is the class for DS3231 that can be used as part of the univRTC library */

#ifndef _RTCDS3231_H_
#define _RTCDS3231_H_

#include <I2CRTC.h>


#define DS3231_ADDRESS 0x68 // I2C address for DS3231
#define DS3231_CLOCKREG 0x00 // Clock register
#define DS3231_ALARM1  0x07 // start of alarm 1 register (seconds)
#define DS3231_CONTROL 0x0E // Control register
#define DS3231_STATUS  0x0F // Status register
#define DS3231_OFFSET  0x10 // Offset register
#define DS3231_TEMPMSB 0x11 // MSB of temperature register
#define DS3231_WDAYBASE 1    // wday range from 1 to 7
#define DS3231_WDAYFIRST true    // wday comes before day of month in clock reg
#define DS3231_CAPABIL  (RTC_CAPABIL_32KHZ|RTC_CAPABIL_1HZ|RTC_CAPABIL_ALARM|RTC_CAPABIL_OFFSET|RTC_CAPABIL_TEMP)


class RTCDS3231: public RTCDSAlarm {
 public:
  RTCDS3231(void);
  void init(byte mode);
  bool isValid(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
  int getTemp(void);
  void setOffset(int offset, byte mode=1);
};
#endif
