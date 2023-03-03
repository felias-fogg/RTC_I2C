/* This is the class for DS1307 that can be used as part of the I2CRTC library */

#ifndef _RTCDS1307_H_
#define _RTCDS1307_H_

#include <I2CRTC.h>

#define DS1307_ADDRESS  0x68     // I2C address for DS1307
#define DS1307_CONTROL  0x07     // Control register
#define DS1307_CLOCKREG 0x00     // Clock register
#define DS1307_WDAYBASE 1        // wday range from 1 to 7
#define DS1307_WDAYFIRST true    // wday comes before day of month in clock reg
#define DS1307_CAPABIL     (RTC_CAPABIL_32KHZ|RTC_CAPABIL_1HZ)


class RTCDS1307: public I2CRTC {
 public:
  RTCDS1307(void);
  void init(byte mode);
  bool isValid(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
};
#endif
