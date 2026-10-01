/* This is the class for RV3028U (using only the Unix clock) that can be used as part of the RTC_I2C library */

#ifndef _RTC_RV3028U_H_
#define _RTC_RV3028U_H_

#include <RTC_RV3028.h>

/// Unix Time 0 register; the RV-3028-U Unix time counter begins at address 0x1B.
#define RV3028_UCLOCK 0x1B

/// Capabilities supported by the RV-3028-U implementation.
#define RV3028U_CAP (RTC_CAP_32KHZ | RTC_CAP_1HZ | RTC_CAP_OFFSET)


/// Class for the [Micro Crystal
/// RV-3028](https://www.microcrystal.com/fileadmin/Media/Products/RTC/Datasheet/RV-3028-C7.pdf), using the Unix clock.
class RV3028U : public RV3028 {
 public:
  /// Initializes the instance for the RV3028U hardware variant.
  RV3028U() {
    _i2caddr = RV3028_ADDRESS;
    _clockreg = RV3028_UCLOCK;
    _wdaybase = 0;
    _wdayfirst = 0;
    _capabilities = RV3028U_CAP;
    _bit7set = RV3028_BIT7;
  }
  bool setAlarm(__attribute__((unused)) byte minute, __attribute__((unused)) byte hour) override { return true; }
  bool setAlarm(__attribute__((unused)) byte minute) override { return true; }
  bool senseAlarm() override { return false; }  // no-op for this RTC
  bool clearAlarm() override { return true; }   // no-op for this RTC
  bool enableAlarm() override { return true; }  // no-op for this RTC
  bool disableAlarm() override { return true; } // no-op for this RTC
  using RTC_I2C::setTime;
  bool setTime(timestamp_t t) override;
  bool setTime(tm timeParts) override;
  timestamp_t getTime() override;
  bool getTime(tm &timeParts) override;
  String getManufacturer() override;
  String getModel() override;

 private:
  bool readUnixTime(timestamp_t &timestamp);
};
#endif
