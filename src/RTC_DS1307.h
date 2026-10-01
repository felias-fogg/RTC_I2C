/* This is the class for DS1307 that can be used as part of the RTC_I2C library */

#ifndef _RTC_DS1307_H_
#define _RTC_DS1307_H_

#include <RTC_I2C.h>

/// 7-bit I2C slave address for the DS1307.
#define DS1307_ADDRESS 0x68
/// Control register; referred to as CONTROL: Control Register in documentation (ADDRESS 0x07).
#define DS1307_CONTROL 0x07
/// Start of the clock/calendar registers; the first clock register is at address 0x00.
#define DS1307_CLOCKREG 0x00
/// No offset/calibration register is provided by the DS1307.
#define DS1307_OFFSET 0xFF
/// Weekday numbering used by the RTC: 1 through 7.
#define DS1307_WDAYBASE 1
/// The weekday register comes before the day-of-month register in the clock register sequence.
#define DS1307_WDAYFIRST true
/// No clock-register bit 7 must be forced when writing time.
#define DS1307_BIT7 0
/// Capabilities supported by the DS1307 implementation.
#define DS1307_CAP (RTC_CAP_32KHZ | RTC_CAP_1HZ)

/// The class for the [Analog Devices
/// DS1307](https://www.analog.com/media/en/technical-documentation/data-sheets/ds1307.pdf)
class DS1307 : public RTC_I2C {
 public:
  /// Initializes the instance for the DS1307 hardware.
  DS1307() {
    _i2caddr = DS1307_ADDRESS;
    _clockreg = DS1307_CLOCKREG;
    _wdaybase = DS1307_WDAYBASE;
    _wdayfirst = DS1307_WDAYFIRST;
    _capabilities = DS1307_CAP;
    _bit7set = DS1307_BIT7;
  }
  bool init(BatteryMode mode = BatteryMode::LEVEL_SWITCHING) override;
  bool isValid() override;
  bool enable32kHz() override;
  bool disable32kHz() override;
  bool enable1Hz() override;
  bool disable1Hz() override;
  String getManufacturer() override;
  String getModel() override;
};
#endif
