# ChangeLog<!--! {#change_log} -->

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) and its stricter, better defined, brother [Common Changelog](https://common-changelog.org/).

This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

<!-- @tableofcontents{XML:1,HTML:1} -->

<!--! @m_footernavigation -->

<!--! @if GITHUB -->

- [ChangeLog](#changelog)
  - [Unreleased](#unreleased)
  - [0.7.0](#070)
  - [V0.6.0 (22.3.2023)](#v060-2232023)
  - [V0.5.2 (8.3.2023)](#v052-832023)
  - [V0.5.1 (7.3.2023)](#v051-732023)
  - [V0.5.0 (7.3.2023)](#v050-732023)
  - [V0.4.1 (6.3.2023)](#v041-632023)
  - [V0.4.0 (5.3.2023)](#v040-532023)
  - [V0.3.2 (4.3.2023)](#v032-432023)
  - [V0.3.1 (4.3.2023)](#v031-432023)
  - [V0.3.0 (4.3.2023)](#v030-432023)
  - [V0.2.5 (3.3.2023)](#v025-332023)
  - [V0.2.4 (3.3.2023)](#v024-332023)
  - [V0.2.3 (2.3.2023)](#v023-232023)
  - [V0.2.2 (2.3.2023)](#v022-232023)
  - [V0.2.1 (28.2.2023)](#v021-2822023)
  - [V0.2.0 (28.2.2023)](#v020-2822023)
  - [V0.1.1 (26.2.2023)](#v011-2622023)
  - [V0.1.0 (25.2.2023)](#v010-2522023)

<!--! @endif -->

***

## [Unreleased]

### Changed

### Added

### Removed

### Fixed

***

## [0.7.0]

### Changed

- Replaced Paul Stoffregen's Time library with the EnviroDIY EpochTime library
  and the standard `time.h` `tm` structure.
- Renamed the base `RTC` class to `RTC_I2C` to avoid conflicts with platform
  core definitions.
- Changed initialization, time-setting, alarm, output, offset, and register
  operations to return success or failure.
- Added strongly typed battery-switching and offset-mode enums, and made
  `begin()` perform device initialization.
- Expanded support from AVR-only to all Arduino architectures.
- Updated and documented the examples, public API, and RTC capability tables.

### Added

- Added functions to get each clock's manufacturer, model, combined make and
  model, and I2C address.
- Added timestamp and `epochTime` overloads plus portable time-conversion
  helpers.
- Added documentation and example-build workflows, Doxygen configuration, and
  ReadMe files for each example.

### Removed

- Removed the blocking-read option from `getTime()`.

### Fixed

- Corrected weekday and calendar-time conversions, including behavior on mbed
  and Raspberry Pi Pico platforms.
- Corrected RTC manufacturer names and propagated I2C communication failures
  through the public API.

***

## V0.6.0 (22.3.2023)

- added: new method getOffset(): returns contents of offset
  register verbatim (i.e., no interpretation as a ppm value)

***

## V0.5.2 (8.3.2023)

- fixed: enable battery switch over (VBATEN) for MCP79410 when setting
         the time
- fixed: implemented setting of time for MCP79410 as described in the
         data sheet, i.e., stop oscillator, set time, start oscillator

***

## V0.5.1 (7.3.2023)

- added hourly recurring alarm (DS1307 and SD2405 do not support them)
- added hourly alarms to testall.ino
- added skipping to 28.2.2100, 23:59:45

***

## V0.5.0 (7.3.2023)

- integrated the strange register addressing  (RTC_CAP_SREGADDR) into
  the base class, so now it is totally transparent to everybody
  using the library.

***

## V0.4.1 (6.3.2023)

- fixed in testall: register number when 'setting' must also be
  massaged when RTC_CAP_SREGADDR.
- added also OFFSET constants for all RTCs that do not have any
- added some explanations read.md about that and about the new example
  iterate.ino

***

## V0.4.0 (5.3.2023)

- added mode=2 for setOffset in order to store the offset value in the
  native format

***

## V0.3.2 (4.3.2023)

- added SD2405
- added possible extra '1' bits in clock register for some RTCs by new
  var _bit7set
- moved constructor into header file

***

## V0.3.1 (4.3.2023)

- added MCP79140
- fixed masking of unused bits in the clock register

***

## V0.3.0 (4.3.2023)

- added update of registers from RAM to EEPROM in RV-3032
- fixed problem with switch-over confusion
- added 3028
- added 3028U (using the Unix counter for timekeeping)
- added timeouts in all loops where we wait for a condition
- renamed library and classes to make them more readable
- new example: iterate

***

## V0.2.5 (3.3.2023)

- added RV-3032
- added 'mode' parameter to init, which can be used to select the battery switch-over mode

***

## V0.2.4 (3.3.2023)

- added optional parameter 'blocking' to getTime, which is false by default;
  if true, then we wait with the return until a fresh second has started.
- added RV-8803
- _wdaybase can now also be 2, which means that the day of the week is
  0-based, but it is now the bit position (for RV-8803)
- changed handling of _wdayfirst in setTime and getTime so that wday-encoding/decoding now
  happens just once
- added section of RTC capabilities in readme

***

## V0.2.3 (2.3.2023)

- checked with new PCF8563 module. This one works flawlessly! The Waveshare one is apparently broken.
- added RV-8523 (well, it is simply a synonym for PCF8523)
- added getTemp method

***

## V0.2.2 (2.3.2023)

- added support for RS5C372
- added a new "capability": strange register address: SREGADDR
  used for RS5C372, who wants to see the register address in
  the upper nibble

***

## V0.2.1 (28.2.2023)

- added support for PCF8563

***

## V0.2.0 (28.2.2023)

- added new methods: enableXXHz, disableXXHz, alarm methods, and a getCapabilities method
- thrown out setAlarm with only minutes since not all RTCs support that, so only hour with minute is
  supported
- added support for DS1337, DS3231, PCF8523
- added example testall, which can be used as a unit test for the different modules

***

## V0.1.1 (26.2.2023)

- renamed from univRTC to I2CRTC
- framework works with DS1307 example

***

## V0.1.0 (25.2.2023)

- first stub with an idea for the API

[Unreleased]: https://github.com/EnviroDIY/RTC_I2C/compare/v0.7.0...HEAD
[0.7.0]: https://github.com/EnviroDIY/RTC_I2C/releases/tag/v0.7.0

<!--! @tableofcontents{HTML:1} -->

<!--! @m_footernavigation -->

<!-- cspell:words I2CRTC SREGADDR Waveshare wday -->
