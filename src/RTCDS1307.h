/* This is the class for DS1307 that can be used as part of the I2CRTC library */

#ifndef _RTCDS1307_H_
#define _RTCDS1307_H_

#include <I2CRTC.h>

#define DS1307_ADDRESS  0x68 // I2C address for DS1307
#define DS1307_CONTROL  0x07 // Control register
#define DS1307_CLOCKREG 0x00 // Control register
#define DS1307_WDAYBASE 1    // wday range from 1 to 7

class RTCDS1307: public I2CRTC {
 public:
  RTCDS1307(void);
  void init(void);
  bool isValid(void);

};
#endif
