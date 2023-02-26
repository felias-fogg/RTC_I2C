# I2CRTC



## A universal API for I2C RTCs

This library provides a universal API to a number of I2C RTCs, which are described in one of my [blog articles](https://hinterm-ziel.de/index.php/2023/02/24/everything-you-wanted-to-know-about-real-time-clocks-but-were-afraid-to-ask/):

* DS1307,
* DS1337,
* DS3231,
* PCF8563,
* PCF8523,
* RS5C372,
* SD2405,
* RV-8523,
* RV-8803, and
* RV-3032.

The interface is minimal, but is just enough to get the basic functionality. And it is implemented in a base class. This means you can even use many RTCs in parallel without caring about what particular model you use. In addition, one can extend the library with methods catering for the particular RTCs, if one wishes to do so.

This library uses Paul Stoffregen's [*Time*](https://github.com/PaulStoffregen/Time) library.

## Methods of the RTC class

| Name           | Parameters                                      | Comments                                                     |
| -------------- | ----------------------------------------------- | ------------------------------------------------------------ |
| `begin`        | none                             | Needs to be called in the beginning. Returns `false` if unsuccessful, otherwise `true`. |
| `init`        | none                                             | Restores state of clock to configuration after power-up. It will also start the clock (if not already running) and clear all invalid flags. |
| `isValid`      | none                                            | Returns `true` if the clock is valid and running, otherwise `false`. |
| `setTime`      | Unix `time_t` `value`                           | Sets the RTC time from provided parameter. Will use the 24 hour format. |
| `setTime` | `tmElements_t` record | sets the RTC time from a `tmElements_t` structure. Note that it is Unix epic based. |
| `getTime`      | none                                            | Returns the current time of the RTC as a Unix `time_t` value. |
| `getTime` | `&tmElements_t` record | Sets all fields of the record from the current time of the RTC. |
| `getRegister`  | `byte` `register address`                       | Returns contents of RTC register.                            |
| `setRegister`  | `byte` `register address,` `byte` `value` | Sets RTC register to value                                                             |


<!-- NOT YET IMPLEMENTED
| `enable32kHz`  | none                                            | Enables output of clock pulses.                              |
| `disable32kHz` | none                                            | Disables output of clock pulses.                             |
| `enable1Hz`    | none                                            | Enables output of 1 Hz signal. Returns `true` when the RTC supports it, otherwise `false`. So far, only RC5C372 cannot generate the 1 Hz signal. |
| `disable1Hz`   | none                                            | Disables 1 Hz output.                                        |
| `setAlarm`     | `int` `seconds`, [`minutes`[, `hour` [,`day`]]] | Set an alarm time, where one either can specify, only seconds, seconds and minutes, seconds, minutes, and hour, or seconds, minutes, hour, and day of the month. Returns `false` if the RTC is not able to set an alarm, otherwise `true`. So far, only DS1307 does not have an alarm. |
| `enableAlarm`  | none                                            | Enable the alarm interrupt. Returns `false` if no alarm is supported by the RTC, otherwise `true`. |
| `disableAlarm` | none                                            | Disables alarm.                                              |
| `setOffset`    | `int` `value` for 0.1 ppm correction steps      | Note that not all RTCs support trimming and they have different step sizes, ranging from 0.1 ppm to 4.3 ppm. The function will try to approximate the passed value as much as possible. Returns `true` when correction can be applied, `false` when the RTC does not have an offset register. |
-->

## Caveats

While this library is a great tool when you want to use different RTCs in parallel (there are not too many use cases for this, though, I believe), you should be aware that the RTCs have often similar I2C addresses. So in order to avoid I2C address clashes, you either need an I2C multiplexer or have to use different I2C buses. The former can be accomplished by a I2C multiplexer such as TCA9548A or PCA9548. The latter can be achieved by using different Wire-alike software I2C instances, such as *SlowSoftWire*. 

