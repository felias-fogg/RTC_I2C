/* This is the class for RS5C372 that can be used as part of the RTC_I2C library */

#ifndef _RTC_RS5C372_H_
#define _RTC_RS5C372_H_

#include <RTC_I2C.h>

// Note that this RTC wants to see the register address (0x0-0xF) in the
// upper nibble of the address byte. Took me a while to find that out.
/// 7-bit I2C slave address for the RS5C372.
#define RS5C372_ADDRESS 0x32
/// Start of the time/calendar registers; the first clock register is at address 0x00.
#define RS5C372_CLOCKREG 0x00
/// Alarm A minute register; the alarm register set begins at address 0x08.
#define RS5C372_ALARMMIN 0x08
/// Alarm A hour register; the alarm register set continues at address 0x09.
#define RS5C372_ALARMHR 0x09
/// Alarm A weekday register; the alarm register set continues at address 0x0A.
#define RS5C372_ALARMWDAYS 0x0A
/// Control 1 register; referred to as Control 1 in documentation (ADDRESS 0x0E).
#define RS5C372_CONTROL1 0x0E
/// Control 2 register; referred to as Control 2 in documentation (ADDRESS 0x0F).
#define RS5C372_CONTROL2 0x0F
/// Offset register; referred to as the clock adjustment/offset register in documentation (ADDRESS 0x07).
#define RS5C372_OFFSET 0x07
/// Weekday numbering used by the RTC: 0 through 6.
#define RS5C372_WDAYBASE 0
/// The weekday register comes before the day-of-month register in the clock register sequence.
#define RS5C372_WDAYFIRST true
/// No clock-register bit 7 must be forced when writing time.
#define RS5C372_BIT7 0
/// Capabilities supported by the RS5C372 implementation.
#define RS5C372_CAP (RTC_CAP_32KHZ | RTC_CAP_1HZ | RTC_CAP_ALARM | RTC_CAP_OFFSET | RTC_CAP_SREGADDR)

/**
 * @brief The class for the [Ricoh RS5C372](http://www.ricoh.com/LSI/product_rtc/2wire/5c372/5c372a-e.pdf)
 *
 * INTRB is used as SQW output for 32 KHz and 1 Hz signals
 * INTRB is the alarm interrupt output
 */
class RS5C372 : public RTC_I2C {
 public:
  /// Initializes the instance for the RS5C372 hardware.
  RS5C372() {
    _i2caddr = RS5C372_ADDRESS;
    _clockreg = RS5C372_CLOCKREG;
    _wdaybase = RS5C372_WDAYBASE;
    _wdayfirst = RS5C372_WDAYFIRST;
    _capabilities = RS5C372_CAP;
    _bit7set = RS5C372_BIT7;
  }
  bool init(BatteryMode mode = BatteryMode::LEVEL_SWITCHING) override;
  bool isValid() override;
  bool setAlarm(byte minute, byte hour) override;
  bool setAlarm(__attribute__((unused)) byte minute) override { return true; } // no-op for this RTC!
  bool enableAlarm() override;
  bool disableAlarm() override;
  bool senseAlarm() override;
  bool clearAlarm() override;
  bool enable32kHz() override;
  bool disable32kHz() override;
  bool enable1Hz() override;
  bool disable1Hz() override;
  /**
   * @copydoc RTC_I2C::setOffset()
   * This RTC has only one calibrated correction mode. Both `OffsetMode::FINE_OFFSET` and `OffsetMode::COARSE_OFFSET`
   * are treated identically.
   */
  bool setOffset(int offset, OffsetMode mode = OffsetMode::FINE_OFFSET) override;
  unsigned int getOffset() override;
  String getManufacturer() override;
  String getModel() override;
};
#endif
