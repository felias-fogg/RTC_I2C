/* This is the class for RV3032 that can be used as part of the RTC_I2C library */

#ifndef _RTC_RV3032_H_
#define _RTC_RV3032_H_

#include <RTC_I2C.h>


/// 7-bit I2C slave address for the RV-3032.
#define RV3032_ADDRESS 0x51
/// Seconds register; referred to as Seconds in documentation (ADDRESS 0x01).
#define RV3032_CLOCKREG 0x01
/// Minutes Alarm register; referred to as Minutes Alarm in documentation (ADDRESS 0x08).
#define RV3032_ALARM 0x08
/// Status register; referred to as Status in documentation (ADDRESS 0x0D).
#define RV3032_STATUS 0x0D
/// Control 1 register; referred to as Control 1 in documentation (ADDRESS 0x10).
#define RV3032_CONTROL 0x10
/// Temperature MSBs register; referred to as Temperature MSBs in documentation (ADDRESS 0x0F).
#define RV3032_TEMP 0x0F
/// Temperature LSBs register; referred to as Temperature LSBs in documentation (ADDRESS 0x0E), containing the EEbusy
/// bit.
#define RV3032_BUSY 0x0E
/// EEPROM PMU register; referred to as EEPROM PMU in documentation (ADDRESS 0xC0).
#define RV3032_COE 0xC0
/// EEPROM Clkout 2 register; referred to as EEPROM Clkout 2 in documentation (ADDRESS 0xC3).
#define RV3032_CLKOUT 0xC3
/// EEPROM Offset register; referred to as EEPROM Offset in documentation (ADDRESS 0xC1).
#define RV3032_OFFSET 0xC1
/// EE Command register; the EEPROM command register is at address 0x3F.
#define RV3032_EECMD 0x3F
/// EE Data register; the EEPROM data register is at address 0x3E.
#define RV3032_EEDATA 0x3E
/// EE Address register; the EEPROM address register is at address 0x3D.
#define RV3032_EEADDR 0x3D
/// No clock-register bit 7 must be forced when writing time.
#define RV3032_BIT7 0
/// Weekday numbering used by the RTC: 0 through 6.
#define RV3032_WDAYBASE 0
/// The weekday register comes before the day-of-month register in the clock register sequence.
#define RV3032_WDAYFIRST true
/// Capabilities supported by the RV-3032 implementation.
#define RV3032_CAP (RTC_CAP_32KHZ | RTC_CAP_1HZ | RTC_CAP_ALARM | RTC_CAP_HOURLY_ALARM | RTC_CAP_OFFSET | RTC_CAP_TEMP)

/// The class for the [Micro Crystal
/// RV-3032](https://www.microcrystal.com/fileadmin/Media/Products/RTC/Datasheet/RV-3032-C7.pdf)
class RV3032 : public RTC_I2C {
 public:
  /// Initializes the instance for the RV3032 hardware.
  RV3032() {
    _i2caddr = RV3032_ADDRESS;
    _clockreg = RV3032_CLOCKREG;
    _wdaybase = RV3032_WDAYBASE;
    _wdayfirst = RV3032_WDAYFIRST;
    _capabilities = RV3032_CAP;
    _bit7set = RV3032_BIT7;
  }
  bool init(BatteryMode mode = BatteryMode::LEVEL_SWITCHING) override;
  bool isValid() override;
  bool setAlarm(byte minute, byte hour) override;
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
   *
   * This RTC has only one calibrated correction mode. Both `OffsetMode::FINE_OFFSET` and `OffsetMode::COARSE_OFFSET`
   * are treated identically.
   *
   * Negative values make the clock faster by 0.2384 ppm/LSB.
   * The range of the internal parameter goes from -32 to +31.
   * This means that possible values for offset range from -768 to +744 corresponding to -7.68 ppm to 7.44 ppm
   */
  bool setOffset(int offset, OffsetMode mode = OffsetMode::FINE_OFFSET) override;
  unsigned int getOffset() override;
  String getManufacturer() override;
  String getModel() override;


 protected:
  /**
   * @brief Update a byte in the EEPROM.
   *
   * @param reg The EEPROM register address to update.
   * @return `true` if the EEPROM byte was successfully updated; otherwise `false`.
   */
  bool updateEEPROMByte(byte reg);
  /**
   * @brief Enable or disable automatic EEPROM refresh (the EERD bit of the control register).
   *
   * @param control The current value of the control register.
   * @param enable `true` to enable automatic EEPROM refresh (clear EERD); `false` to disable it (set EERD).
   * @return `true` if the control register was successfully updated; otherwise `false`.
   */
  bool setEEPROMRefresh(byte control, bool enable);
};
#endif

// cSpell:ignore EECMD EEDATA EEADDR
