/* This is the class for RV8803 that can be used as part of the RTC_I2C library */

/* Note that the CLKOUT pin can only be disabled by holding CLKOE low. For this reason
 * the two disable functions are no-ops.
 * In addition, when setting time, the RESET bit must be active. When getting time,
 * we need to read twice if the seconds equal 59.
 */

#ifndef _RTC_RV8803_H_
#define _RTC_RV8803_H_

#include <RTC_I2C.h>


/// 7-bit I2C slave address for the RV-8803.
#define RV8803_ADDRESS 0x32
/// Seconds register; the clock/calendar register sequence begins at address 0x00.
#define RV8803_CLOCKREG 0x00
/// Minutes Alarm register; the alarm register set begins at address 0x08.
#define RV8803_ALARM 0x08
/// CLKOUT register; referred to as the CLKOUT register in documentation (ADDRESS 0x0D).
#define RV8803_CLKOUT 0x0D
/// Status register; referred to as the Status register in documentation (ADDRESS 0x0E).
#define RV8803_STATUS 0x0E
/// Control register; referred to as the Control register in documentation (ADDRESS 0x0F).
#define RV8803_CONTROL 0x0F
/// Offset register; referred to as the Digital Offset register in documentation (ADDRESS 0x2C).
#define RV8803_OFFSET 0x2C
/// Sentinel selecting the RTC's one-hot weekday encoding (Sunday is bit 0 through Saturday at bit 6).
#define RV8803_WDAYBASE 2
/// The weekday register comes before the day-of-month register in the clock register sequence.
#define RV8803_WDAYFIRST true
/// No clock-register bit 7 must be forced when writing time.
#define RV8803_BIT7 0
/// Capabilities supported by the RV-8803 implementation.
#define RV8803_CAP (RTC_CAP_32KHZ | RTC_CAP_1HZ | RTC_CAP_ALARM | RTC_CAP_HOURLY_ALARM | RTC_CAP_OFFSET)

/// The class for the [Micro Crystal
/// RV-8803](https://www.microcrystal.com/fileadmin/Media/Products/RTC/Datasheet/RV-8803-C7.pdf)
class RV8803 : public RTC_I2C {
 public:
  /// Initializes the instance for the RV8803 hardware.
  RV8803() {
    _i2caddr = RV8803_ADDRESS;
    _clockreg = RV8803_CLOCKREG;
    _wdaybase = RV8803_WDAYBASE;
    _wdayfirst = RV8803_WDAYFIRST;
    _capabilities = RV8803_CAP;
    _bit7set = RV8803_BIT7;
  }
  bool init(BatteryMode mode = BatteryMode::LEVEL_SWITCHING) override;
  bool isValid() override;
  using RTC_I2C::setTime;
  bool setTime(timestamp_t t) override;
  bool setTime(tm timeParts) override;
  timestamp_t getTime() override;
  bool getTime(tm &timeParts) override;
  bool setAlarm(byte minute, byte hour) override;
  bool setAlarm(byte minute) override;
  bool senseAlarm() override;
  bool clearAlarm() override;
  bool enableAlarm() override;
  bool disableAlarm() override;
  bool enable32kHz() override;
  bool enable1Hz() override;
  /**
   * @copydoc RTC_I2C::setOffset()
   *
   * This RTC has only one calibrated correction mode. Both `OffsetMode::FINE_OFFSET` and `OffsetMode::COARSE_OFFSET`
   * are treated identically.
   *
   * Negative values make the clock faster by 0.2384 ppm/LSB
   * The range of the internal parameter goes from -32 to +31.
   * This means that possible values for offset range from -768 to +744 corresponding to -7.68 ppm to 7.44 ppm
   */
  bool setOffset(int offset, OffsetMode mode = OffsetMode::FINE_OFFSET) override;
  unsigned int getOffset() override;
  String getManufacturer() override;
  String getModel() override;
  bool setRegister(byte reg, byte val) override;

 private:
  bool finishWriteAccess(byte reg);
  bool writeTimeRegisters(tm timeParts);
};
#endif
