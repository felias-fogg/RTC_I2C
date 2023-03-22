/* This is the class for RV8803 that can be used as part of the RTC_I2C library */

/* Note that the CLKOUT pin can only be disabled by holding CLKOE low. For this reason
 * the two disable functions are no-ops.
 * In addition, when setting time, the RESET bit must be active. When getting time, 
 * we need to read twice if the seconds equal 59.
 */

#ifndef _RTC_RV8803_H_
#define _RTC_RV8803_H_

#include <RTC_I2C.h>


#define RV8803_ADDRESS 0x32 // I2C address for RV8803
#define RV8803_CLOCKREG 0x00 // Clock register 
#define RV8803_ALARM   0x08 // Alarm minutes register
#define RV8803_CLKOUT 0x0D // Clockout register
#define RV8803_STATUS 0x0E// Status register
#define RV8803_CONTROL 0x0F// Control register
#define RV8803_OFFSET  0x2C // Offset register
#define RV8803_WDAYBASE 2    // wday range from 0 to 6, but it is now the 1-bit at this bit position!
#define RV8803_WDAYFIRST true    // wday comes after day of month in clock reg
#define RV8803_BIT7 0
#define RV8803_CAP  (RTC_CAP_32KHZ|RTC_CAP_1HZ|RTC_CAP_ALARM|RTC_CAP_HOURLY_ALARM|RTC_CAP_OFFSET)


class RV8803: public RTC {
 public:
  RV8803(void) {
  _i2caddr = RV8803_ADDRESS;
  _clockreg = RV8803_CLOCKREG;
  _wdaybase = RV8803_WDAYBASE;
  _wdayfirst = RV8803_WDAYFIRST;
  _capabilities = RV8803_CAP;
  _bit7set = RV8803_BIT7;
};
  void init(byte mode=1);
  bool isValid(void);
  void setTime(time_t t);
  void setTime(tmElements_t tm);
  time_t getTime(bool blocking=false);
  void getTime(tmElements_t &tm, bool blocking=false);
  void setAlarm(byte minute, byte hour);
  void setAlarm(byte minute);
  bool senseAlarm(void);
  void clearAlarm(void);
  void enableAlarm(void);
  void disableAlarm(void);
  void enable32kHz(void);
  void enable1Hz(void);
  void setOffset(int offset, byte mode=1);
  unsigned int getOffset(void);
};
#endif
