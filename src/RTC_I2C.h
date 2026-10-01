/*****************************************************************************************
 *
 * This library provides a universal interface to many different RTCs making it possible
 * to use these RTCs in parallel, not caring about their particular features.
 *
 * License: MIT
 *
 *
 *****************************************************************************************/

#ifndef _RTC_I2C_H_
#define _RTC_I2C_H_

#include <Arduino.h>
// #include <TimeLib.h>
#include <Wire.h>
#include <time.h>
#include <EpochTime.h>


/// Number of clock/calendar bytes transferred by the common RTC implementation.
#define TIMEBYTES 7
/// Capability flag: the RTC can generate a 32 kHz signal.
#define RTC_CAP_32KHZ 0x01
/// Capability flag: the RTC can generate a 1 Hz signal.
#define RTC_CAP_1HZ 0x02
/// Capability flag: the RTC has alarm functionality to set hour and minute.
#define RTC_CAP_ALARM 0x04
/// Capability flag: the RTC can raise an alarm every hour.
#define RTC_CAP_HOURLY_ALARM 0x08
/// Capability flag: the RTC has an offset/calibration register.
#define RTC_CAP_OFFSET 0x10
/// Capability flag: the RTC has a temperature sensor.
#define RTC_CAP_TEMP 0x20
/// Capability flag: the RTC uses the library's special upper-nibble register-address format.
#define RTC_CAP_SREGADDR 0x40
/// Capability flag: the RTC requires a STOP between the register-address write and subsequent read.
#define RTC_CAP_STOP_BEFORE_READ 0x80

#ifndef SECONDS_IN_DAY
/// Number of seconds in a day.
#define SECONDS_IN_DAY 86400L
#endif

/// The mode for switchover of the backup battery.
enum class BatteryMode : int8_t {
  SWITCHING_DISABLED, ///< Battery switch-over function is disabled, only VDD is used
  LEVEL_SWITCHING, ///< Level Switching Mode (LSM). Switchover when VDD < threshold AND VBACKUP > threshold.  Use with a
                   ///< standard coin cell battery.
  DIRECT_SWITCHING ///< Direct Switching Mode (DSM). Switchover when VDD < VBACKUP.  Use when charging a rechargeable
                   ///< battery.
};

/**
 * @brief Selects how a clock calibration offset or trim is interpreted.
 *
 * See the specific RTC documentation for details on the correction frequencies that are considered to be coarse, fine,
 * or very fine.
 */
enum class OffsetMode : byte {
  COARSE_OFFSET = 0, ///< Apply the offset as a coarse correction.
  FINE_OFFSET = 1,   ///< Apply the offset as a fine correction.
  RAW_OFFSET = 2     ///< Treat the given offset as the exact value to write to the RTC's offset/calibration-register.
};


/// A generic RTC base class
class RTC_I2C {
 public:
  /**
   * @brief Initialize the I2C interface and establish communication with the RTC.
   *
   * If communication has already been established, subsequent calls return without
   * reinitializing the interface. Once the RTC responds, the device-specific `init()`
   * function is called.
   *
   * @param wi The I2C interface to use.
   * @param mode The battery switchover mode to configure during initialization.  This only applies to RTCs that support
   * battery switchover - generally only those that support charging a rechargeable backup battery.  Ignored if
   * unsupported.
   * @return `true` if the RTC address is valid and the device responds on I2C; otherwise `false`.
   */
  bool begin(TwoWire *wi = &Wire, BatteryMode mode = BatteryMode::LEVEL_SWITCHING);

  /**
   * @brief Initialize device-specific RTC settings.
   * @param mode The battery switchover mode to configure.  This only applies to RTCs that support battery switchover -
   * generally only those that support charging a rechargeable backup battery.  Ignored if unsupported.
   * @return `true` when the required I2C communication succeeds; otherwise `false`.
   */
  virtual bool init(BatteryMode mode = BatteryMode::LEVEL_SWITCHING) = 0;

  /**
   * @brief Check whether the RTC is operating normally.
   * @return `true` when the RTC reports a valid operating state; otherwise `false`.
   */
  virtual bool isValid() = 0;

  /**
   * @brief Set the RTC from a Unix timestamp.
   * @param unixTimestamp The Unix timestamp to write to the RTC.
   * @return `true` when the required I2C communication succeeds; otherwise `false`.
   */
  virtual bool setTime(timestamp_t unixTimestamp);

  /**
   * @brief Set the RTC from an `epochTime` object.
   * @param eTime The time value to write to the RTC.
   * @return `true` when the required I2C communication succeeds; otherwise `false`.
   */
  virtual bool setTime(epochTime eTime);

  /**
   * @brief Set the RTC from a `tm` calendar-time structure.
   *
   * The base implementation writes the seconds, minutes, hours, weekday, day,
   * month, and year fields using the register layout described by the derived RTC.
   *
   * @param timeParts The calendar time to write to the RTC.
   * @return `true` when the I2C write succeeds; otherwise `false`.
   */
  virtual bool setTime(tm timeParts);

  /**
   * @brief Read the RTC as a Unix timestamp.
   * @return The current RTC time as a Unix timestamp.
   */
  virtual timestamp_t getTime();

  /**
   * @brief Read the RTC into a `tm` calendar-time structure.
   * @param[out] timeParts The calendar-time structure to populate.
   * @return `true` when the register-address write and requested 7-byte read succeed; otherwise `false`.
   */
  virtual bool getTime(tm &timeParts);

  /**
   * @brief Enable the RTC's 32 kHz output; a no-op for RTCs that do not have a 32kHz output.
   * @return `true` when the operation succeeds or is a no-op; otherwise `false`.
   */
  virtual bool enable32kHz() { return true; }

  /**
   * @brief Disable the RTC's 32 kHz output; a no-op for RTCs that do not have a 32kHz output.
   * @return `true` when the operation succeeds or is a no-op; otherwise `false`.
   */
  virtual bool disable32kHz() { return true; }

  /**
   * @brief Enable the RTC's 1 Hz output; a no-op for RTCs that do not have a 1Hz output.
   * @return `true` when the operation succeeds or is a no-op; otherwise `false`.
   */
  virtual bool enable1Hz() { return true; }

  /**
   * @brief Disable the RTC's 1 Hz output; a no-op for RTCs that do not have a 1Hz output.
   * @return `true` when the operation succeeds or is a no-op; otherwise `false`.
   */
  virtual bool disable1Hz() { return true; }

  /**
   * @brief Configure an alarm that matches a specific hour and minute; a no-op for RTCs that do not have an alarm.
   * @param minute The minute at which the alarm should match.
   * @param hour The hour at which the alarm should match.
   * @return `true` when the operation succeeds or is a no-op; otherwise `false`.
   */
  virtual bool setAlarm(__attribute__((unused)) byte minute, __attribute__((unused)) byte hour) { return true; }

  /**
   * @brief Configure an alarm that matches a specific minute; a no-op for RTCs that do not have an alarm.
   * @param minute The minute at which the alarm should match.
   * @return `true` when the operation succeeds or is a no-op; otherwise `false`.
   */
  virtual bool setAlarm(__attribute__((unused)) byte minute) { return true; }

  /**
   * @brief Enable the RTC alarm interrupt or alarm function; a no-op for RTCs that do not have an alarm.
   * @return `true` when the operation succeeds or is a no-op; otherwise `false`.
   */
  virtual bool enableAlarm() { return true; }

  /**
   * @brief Disable the RTC alarm interrupt or alarm function; a no-op for RTCs that do not have an alarm.
   * @return `true` when the operation succeeds or is a no-op; otherwise `false`.
   */
  virtual bool disableAlarm() { return true; }

  /**
   * @brief Check whether the RTC alarm condition is asserted.
   * @return `true` if the RTC reports an asserted alarm; otherwise `false`.
   */
  virtual bool senseAlarm() { return false; }

  /**
   * @brief Clear the RTC alarm condition; a no-op for RTCs that do not have an alarm.
   * @return `true` when the operation succeeds or is a no-op; otherwise `false`.
   */
  virtual bool clearAlarm() { return true; }

  /**
   * @brief Apply a calibration offset to the RTC.
   *
   * The interpretation of `offset` is selected by `mode`. The base implementation
   * does nothing for RTCs without a calibration register.
   *
   * @param offset The calibration correction to apply.
   * @param mode Selects whether the correction is interpreted as a coarse, fine, or raw offset.
   * @return `true` when the I2C write succeeds or the operation is a no-op; otherwise `false`.
   */
  virtual bool setOffset(__attribute__((unused)) int offset,
                         __attribute__((unused)) OffsetMode mode = OffsetMode::FINE_OFFSET) {
    return true;
  }

  /**
   * @brief Read the RTC calibration offset or zero for RTCs without a calibration register.
   * @return The calibration offset in the RTC-specific library representation.
   */
  virtual unsigned int getOffset() { return 0; }

  /**
   * @brief Read the RTC's temperature measurement or `-128` for RTCs without a temperature sensor.
   * @return The RTC temperature, or `-128` when no temperature sensor is available.
   */
  virtual int getTemp() { return -128; }

  /**
   * @brief Get the RTC manufacturer name.
   * @return The manufacturer name.
   */
  virtual String getManufacturer() = 0;

  /**
   * @brief Get the RTC model name.
   * @return The model name.
   */
  virtual String getModel() = 0;

  /**
   * @brief Get the RTC manufacturer and model as one string.
   * @return The manufacturer and model separated by a space.
   */
  virtual String getMakeModel();

  /**
   * @brief Write one RTC register.
   *
   * The register address is adjusted automatically for RTCs that use the library's
   * special register-address format.
   *
   * @param reg The RTC register address.
   * @param val The byte value to write.
   * @return `true` when the I2C transmission succeeds; otherwise `false`.
   */
  virtual bool setRegister(byte reg, byte val);

  /**
   * @brief Read one RTC register.
   *
   * The register address is adjusted automatically for RTCs that use the library's
   * special register-address format. A failed I2C transaction returns `0xFF`.
   *
   * @param reg The RTC register address.
   * @return The value read from the register, or `0xFF` if the I2C transaction fails.
   */
  virtual byte getRegister(byte reg);

  /**
   * @brief Get the capability flags supported by this RTC.
   *
   * The returned value is a bitmask composed of the `RTC_CAP_*` constants.
   *
   * @return The RTC capability bitmask.
   */
  virtual byte getCapabilities() { return _capabilities; }

  /**
   * @brief Get the RTC's I2C address.
   * @return The 7-bit I2C address configured for this RTC.
   */
  virtual uint8_t getAddress() { return _i2caddr; }

 protected:
  /**
   * @brief Read one RTC register and report I2C failures separately from the byte value.
   * @param reg The RTC register address.
   * @param[out] val The value read from the register when the transaction succeeds.
   * @return `true` when the complete I2C transaction succeeds; otherwise `false`.
   *
   * This differs from `getRegister()` in that it returns a bool based on success and requires a reference to store the
   * read value.
   */
  bool readRegister(byte reg, byte &val);

  /**
   * @brief Convert a packed BCD byte to a binary value.
   * @param val The packed BCD value.
   * @return The corresponding binary value.
   */
  static byte bcd2bin(byte val) __attribute__((weak)) { return val - 6 * (val >> 4); }

  /**
   * @brief Convert a binary value to a packed BCD byte.
   * @param val The binary value to convert.
   * @return The corresponding packed BCD value.
   */
  static byte bin2bcd(byte val) __attribute__((weak)) { return val + 6 * (val / 10); }

  /**
   * @brief Decode a weekday bit field into the `tm` weekday numbering.
   * @param bits The RTC weekday bit field.
   * @return The decoded weekday in the `tm` range 0 through 6.
   */
  static byte decodewday(byte bits);

  /// Internal reference to the I2C address
  byte _i2caddr = 0;
  /// Internal reference to the I2C bus object
  TwoWire *_wire = NULL;
  /// Flag indicating whether the RTC has been successfully started
  bool _started = false;
  /// where clock reg starts
  byte _clockreg;
  /// base of weekday counting, either 0 (days range from 0-6) or 1 (days range from 1-7)
  byte _wdaybase;
  /// true when weekday comes before day in clock register
  bool _wdayfirst;
  /// lists all capabilities of this RTC
  byte _capabilities;
  /// if the 7th bit in a byte of the clock register must be set (bit 0=sec, bit 1=min, ...)
  byte _bit7set;
};

/// Base class for Alarms for DS-family RTCs
class DSAlarm : public RTC_I2C {
 public:
  /**
   * @brief Configure a DS-family alarm for a specific hour and minute.
   *
   * The alarm matches the requested hour and minute and ignores the day by using
   * the alarm's always-match day setting.
   *
   * @param minute The minute at which the alarm should match.
   * @param hour The hour at which the alarm should match.
   * @return `true` if the alarm was successfully set; otherwise `false`.
   */
  bool setAlarm(byte minute, byte hour) override;

  /**
   * @brief Configure a DS-family alarm for a specific minute of every hour.
   *
   * The hour and day fields are configured as always-match fields.
   *
   * @param minute The minute at which the alarm should match.
   * @return `true` if the alarm was successfully set; otherwise `false`.
   */
  bool setAlarm(byte minute) override;

  /**
   * @brief Enable the DS-family alarm.
   * @return `true` if the alarm was successfully enabled; otherwise `false`.
   */
  bool enableAlarm() override;

  /**
   * @brief Disable the DS-family alarm.
   * @return `true` if the alarm was successfully disabled; otherwise `false`.
   */
  bool disableAlarm() override;

  /**
   * @brief Check the DS-family alarm status flag.
   *
   * @return `true` if the alarm status bit is set; otherwise `false`.
   */
  bool senseAlarm() override;

  /**
   * @brief Clear the DS-family alarm status flag.
   * @return `true` if the alarm status flag was successfully cleared; otherwise `false`.
   */
  bool clearAlarm() override;
};

/// Base class for Alarms for PCF-family RTCs
class PCFAlarm : public RTC_I2C {
 public:
  /**
   * @brief Configure a PCF-family alarm for a specific hour and minute.
   *
   * The day and weekday fields are configured as always-match fields.
   *
   * @param minute The minute at which the alarm should match.
   * @param hour The hour at which the alarm should match.
   * @return `true` if the alarm was successfully set; otherwise `false`.
   */
  bool setAlarm(byte minute, byte hour) override;

  /**
   * @brief Configure a PCF-family alarm for a specific minute of every hour.
   *
   * The hour, day, and weekday fields are configured as always-match fields.
   *
   * @param minute The minute at which the alarm should match.
   * @return `true` if the alarm was successfully set; otherwise `false`.
   */
  bool setAlarm(byte minute) override;

  /**
   * @brief Check the PCF-family alarm status flag.
   *
   * @return `true` if the alarm status bit is set; otherwise `false`.
   */
  bool senseAlarm() override;

  /**
   * @brief Clear the PCF-family alarm status flag.
   * @return `true` if the alarm status flag was successfully cleared; otherwise `false`.
   */
  bool clearAlarm() override;
};
#endif
