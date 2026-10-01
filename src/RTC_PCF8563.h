/* This is the class for PCF8563 that can be used as part of the RTC_I2C library */

#ifndef _RTC_PCF8563_H_
#define _RTC_PCF8563_H_

#include <RTC_I2C.h>


/// 7-bit I2C slave address for the PCF8563.
#define PCF8563_ADDRESS 0x51
/// Seconds and clock-integrity register; referred to as VL_seconds in documentation (ADDRESS 0x02).
#define PCF8563_CLOCKREG 0x02
/// Control and status register 1; referred to as Control_status_1 in documentation (ADDRESS 0x00).
#define PCF8563_CONTROL 0x00
/// CLKOUT control register; referred to as CLKOUT_control in documentation (ADDRESS 0x0D).
#define PCF8563_CLKOUT 0x0D
/// No offset/calibration register is provided by the PCF8563.
#define PCF8563_OFFSET 0xFF
/// Weekday numbering used by the RTC: 0 through 6.
#define PCF8563_WDAYBASE 0
/// The weekday register comes after the day-of-month register in the clock register sequence.
#define PCF8563_WDAYFIRST false
/// No clock-register bit 7 must be forced when writing time.
#define PCF8563_BIT7 0
/// Capabilities supported by the PCF8563 implementation.
#define PCF8563_CAP (RTC_CAP_32KHZ | RTC_CAP_1HZ | RTC_CAP_ALARM | RTC_CAP_HOURLY_ALARM)

/// The class for the [NXP Semiconductors PCF8563](https://www.nxp.com/docs/en/data-sheet/PCF8563.pdf)
class PCF8563 : public PCFAlarm {
 public:
  /// Initializes the instance for the PCF8563 hardware.
  PCF8563() {
    _i2caddr = PCF8563_ADDRESS;
    _clockreg = PCF8563_CLOCKREG;
    _wdaybase = PCF8563_WDAYBASE;
    _wdayfirst = PCF8563_WDAYFIRST;
    _capabilities = PCF8563_CAP;
    _bit7set = PCF8563_BIT7;
  }
  bool init(BatteryMode mode = BatteryMode::LEVEL_SWITCHING) override;
  bool isValid() override;
  bool enableAlarm() override;
  bool disableAlarm() override;
  bool enable32kHz() override;
  bool disable32kHz() override;
  bool enable1Hz() override;
  bool disable1Hz() override;
  String getManufacturer() override;
  String getModel() override;
};
#endif
