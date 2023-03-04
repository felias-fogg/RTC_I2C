/* This is the class for RV3028U (using only the Unix clock) that can be used as part of the RTC_I2C library */

#ifndef _RTC_RV3028U_H_
#define _RTC_RV3028U_H_

#include <RTC_RV3028.h>
#include <util/delay.h>

#define RV3028_UCLOCK 0x1B

#define RV3028U_CAP  (RTC_CAP_32KHZ|RTC_CAP_1HZ|RTC_CAP_OFFSET)


class RV3028U: public RV3028 {
 public:
  RV3028U(void);
  void setAlarm(__attribute__ ((unused)) byte minute, __attribute__ ((unused)) byte hour) { };
  bool senseAlarm(void) { return false; };
  void clearAlarm(void) { };
  void enableAlarm(void) { };
  void disableAlarm(void) { };
  void setTime(time_t t);
  void setTime(tmElements_t tm);
  time_t getTime(bool blocking = false);
  void getTime(tmElements_t &tm, bool blocking = false);
};
#endif
