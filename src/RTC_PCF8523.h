/* This is the class for PCF8523 that can be used as part of the RTC_I2C library */
/* When 1Hz or 32kHz signals are enables, they can be sensed at CLKOUT and INT1.
 * The alarm interrupt can be sensed at INT1. When alarm is active, then there is no
 * 1Hz or 32kHz output on INT1.
 */

#ifndef _RTC_PCF8523_H_
#define _RTC_PCF8523_H_

#include <RTC_I2C.h>


/// 7-bit I2C slave address for the PCF8523.
#define PCF8523_ADDRESS 0x68
/// Seconds and clock integrity status register; referred to as Seconds in documentation (ADDRESS 0x03).
#define PCF8523_CLOCKREG 0x03
/// Control and status register 1; referred to as Control_1 in documentation (ADDRESS 0x00).
#define PCF8523_CONTROL 0x00
/// Offset calibration register; referred to as Offset in documentation (ADDRESS 0x0E).
#define PCF8523_OFFSET 0x0E
/// Timer and clock-output control register; referred to as Tmr_CLKOUT_ctrl in documentation (ADDRESS 0x0F).
#define PCF8523_CLKOUT 0x0F
/// Weekday numbering used by the RTC: 0 through 6.
#define PCF8523_WDAYBASE 0
/// The weekday register comes after the day-of-month register in the clock register sequence.
#define PCF8523_WDAYFIRST false
/// No clock-register bit 7 must be forced when writing time.
#define PCF8523_BIT7 0
/// Capabilities supported by the PCF8523 implementation.
#define PCF8523_CAP                                                                                                    \
  (RTC_CAP_32KHZ | RTC_CAP_1HZ | RTC_CAP_ALARM | RTC_CAP_HOURLY_ALARM | RTC_CAP_OFFSET | RTC_CAP_STOP_BEFORE_READ)

/// The class for the [NXP Semiconductors PCF8523](https://www.nxp.com/docs/en/data-sheet/PCF8523.pdf)
class PCF8523 : public PCFAlarm {
 public:
  /// Initializes the instance for the PCF8523 hardware.
  PCF8523() {
    _i2caddr = PCF8523_ADDRESS;
    _clockreg = PCF8523_CLOCKREG;
    _wdaybase = PCF8523_WDAYBASE;
    _wdayfirst = PCF8523_WDAYFIRST;
    _capabilities = PCF8523_CAP;
    _bit7set = PCF8523_BIT7;
  }
  bool init(BatteryMode mode = BatteryMode::LEVEL_SWITCHING) override;
  bool isValid() override;
  bool enableAlarm() override;
  bool disableAlarm() override;
  bool enable32kHz() override;
  bool disable32kHz() override;
  bool enable1Hz() override;
  bool disable1Hz() override;
  /**
   * @copydoc RTC_I2C::setOffset()
   * Negative values make the clock faster by roughly 4.0 ppm/LSB
   * In OffsetMode::COARSE_OFFSET, 1 LSB is roughly 4.34 ppm;
   * In OffsetMode::FINE_OFFSET, 1 LSB is roughly 4.06 ppm.
   * The range goes from -64 to +63.
   */
  bool setOffset(int offset, OffsetMode mode = OffsetMode::FINE_OFFSET) override;
  unsigned int getOffset() override;
  String getManufacturer() override;
  String getModel() override;
};
#endif
