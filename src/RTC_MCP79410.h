/* This is the class for MCP79410 that can be used as part of the RTC_I2C library */

#ifndef _RTC_MCP79410_H_
#define _RTC_MCP79410_H_

#include <RTC_I2C.h>


/// 7-bit I2C slave address for the MCP79410.
#define MCP79410_ADDRESS 0x6F
/// Timekeeping seconds register; referred to as RTCSEC: TIMEKEEPING SECONDS VALUE REGISTER in documentation (ADDRESS
/// 0x00).
#define MCP79410_CLOCKREG 0x00
/// Alarm 0 seconds register; referred to as ALMxSEC: ALARM0/1 SECONDS VALUE REGISTER in documentation (ADDRESS 0x0A).
#define MCP79410_ALARM 0x0A
/// Timekeeping weekday register; referred to as RTCWKDAY: TIMEKEEPING WEEKDAY VALUE REGISTER in documentation (ADDRESS
/// 0x03).
#define MCP79410_STATUS 0x03
/// RTCC control register; referred to as CONTROL: RTCC CONTROL REGISTER in documentation (ADDRESS 0x07).
#define MCP79410_CONTROL 0x07
/// Oscillator trim register; referred to as OSCTRIM: OSCILLATOR DIGITAL TRIM REGISTER in documentation (ADDRESS 0x08).
#define MCP79410_OFFSET 0x08
/// Weekday numbering used by the RTC: 1 through 7.
#define MCP79410_WDAYBASE 1
/// The weekday register comes before the day-of-month register in the clock register sequence.
#define MCP79410_WDAYFIRST true
/// No clock-register bit 7 must be forced when writing time.
#define MCP79410_BIT7 0
/// Capabilities supported by the MCP79410 implementation.
#define MCP79410_CAP (RTC_CAP_32KHZ | RTC_CAP_1HZ | RTC_CAP_ALARM | RTC_CAP_HOURLY_ALARM | RTC_CAP_OFFSET)

/// The class for the [Microchip
/// MCP79410](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/20005010H.pdf)
class MCP79410 : public RTC_I2C {
 public:
  /// Initializes the instance for the MCP79410 hardware.
  MCP79410() {
    _i2caddr = MCP79410_ADDRESS;
    _clockreg = MCP79410_CLOCKREG;
    _wdaybase = MCP79410_WDAYBASE;
    _wdayfirst = MCP79410_WDAYFIRST;
    _capabilities = MCP79410_CAP;
    _bit7set = MCP79410_BIT7;
  }
  bool init(BatteryMode mode = BatteryMode::LEVEL_SWITCHING) override;
  bool isValid() override;
  using RTC_I2C::setTime;
  bool setTime(timestamp_t t) override;
  bool setTime(tm timeParts) override;
  bool setAlarm(byte minute, byte hour) override; // here we can only set the alarm til next match
  bool setAlarm(byte minute) override;
  bool senseAlarm() override;
  bool clearAlarm() override;
  bool enableAlarm() override;
  bool disableAlarm() override;
  bool enable32kHz() override;
  bool disable32kHz() override;
  bool enable1Hz() override;
  bool disable1Hz() override;
  /**
   * @copydoc RTC_I2C::setOffset()
   * Negative values make the clock faster by roughly 1 ppm/LSB in the normal calibrated correction modes. The range of
   * the internal parameter goes from -128 to +127, but they use apparently sign + magnitude instead of 2ers complement.
   *
   * Only raw and fine calibrations are supported, coarse calibration will screw up the SQW output and is too coarse
   * anyways.
   */
  bool setOffset(int offset, OffsetMode mode = OffsetMode::FINE_OFFSET) override;
  unsigned int getOffset() override;
  String getManufacturer() override;
  String getModel() override;
};
#endif
