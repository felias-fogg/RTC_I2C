/* This is the class for RS5C372 that can be used as part of the RTC_I2C library */

#ifndef _RTC_RS5C372_H_
#define _RTC_RS5C372_H_

#include <RTC_I2C.h>

// Note that this RTC wants to see the register address (0x0-0xF) in the
// upper nibble of the address byte. Took me a while to find that out.
#define RS5C372_ADDRESS 0x32 // I2C address for RS5C372
#define RS5C372_CLOCKREG 0x00 // Clock register
#define RS5C372_ALARMMIN   0x08  // Alarm A register for minutes
#define RS5C372_ALARMHR   0x09  // Alarm A register for the hour
#define RS5C372_ALARMWDAYS   0x0A  // Alarm A register for week days
#define RS5C372_CONTROL1 0x0E // Control 1 register
#define RS5C372_CONTROL2 0x0F // Control 2 register
#define RS5C372_OFFSET  0x07 // Offset register
#define RS5C372_WDAYBASE 0    // wday range from 0 to 6
#define RS5C372_WDAYFIRST true    // wday comes before day of month in clock reg
#define RS5C372_BIT7 0
#define RS5C372_CAP  (RTC_CAP_32KHZ|RTC_CAP_1HZ|RTC_CAP_ALARM|RTC_CAP_OFFSET|RTC_CAP_SREGADDR)


/* INTRB is used as SQW output fir 32 KHz and 1 Hz signals
 * INTRB is the alarm interrupt output */
class RS5C372: public RTC {
 public:
  RS5C372(void)  {
  _i2caddr = RS5C372_ADDRESS;
  _clockreg = RS5C372_CLOCKREG;
  _wdaybase = RS5C372_WDAYBASE;
  _wdayfirst = RS5C372_WDAYFIRST;
  _capabilities = RS5C372_CAP;
  _bit7set = RS5C372_BIT7;
}
  void init(byte mode=1);
  bool isValid(void);
  void setAlarm(byte minute, byte hour);
  void setAlarm(__attribute__ ((unused)) byte minute) { }; // no-op for this RTC!
  void enableAlarm(void);
  void disableAlarm(void);
  bool senseAlarm(void);
  void clearAlarm(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
  void setOffset(int offset, byte mode=1);
  unsigned int getOffset(void);
};
#endif
