# univRTC



## A universal API for RTCs

This library provides a universal API to a number of RTCs, which are described in one of my [blog articles](https://hinterm-ziel.de/index.php/2023/02/24/everything-you-wanted-to-know-about-real-time-clocks-but-were-afraid-to-ask/):

* DS1307,
* DS1337,
* DS3231,
* PCF8563,
* PCF8523,
* RS5C372,
* SD2405AL,
* RV-8523,
* RV-8803, and
* RV-3032.

The interface is minimal, but is just enough to get the basic functionality. And the interface uses a base class with the minimal interface. This means you can even use many RTCs in parallel without caring about what particular model you use. In addition, one can extend the library with methods catering for the particular RTCs, if one wishes to do so.

This library uses the [*Time*](https://github.com/PaulStoffregen/Time) library.

## Methods of the RTC class

| Name           | Parameters                                      | Comments                                                     |
| -------------- | ----------------------------------------------- | ------------------------------------------------------------ |
| `begin`        | `Wire` instance                                 | Needs to be called in the beginning. Parameter defaults to `Wire`. Returns `false` if unsuccessful, otherwise `true`. |
| `start`        | none                                            | Starts clock (might do nothing on some RTCs). Will also clear any invalidness flag. |
| `stop`         | none                                            | Stops clock (might do nothing on some RTCs). Returns `true` if successful, otherwise `false`. |
| `isValid`      | none                                            | Returns `true` if the clock is valid and running, otherwise `false`. |
| `setTime`      | Unix `time_t` `value`                           | Sets the RTC time from provided parameter. Will use the 24 hour format. |
| `getTime`      | none                                            | Returns the current time of the RTC as a Unix `time_t` value. |
| `enable32kHz`  | none                                            | Enables output of clock pulses.                              |
| `disable32kHz` | none                                            | Disables output of clock pulses.                             |
| `enable1Hz`    | none                                            | Enables output of 1 Hz signal. Returns `true` when the RTC supports it, otherwise `false`. So far, only RC5C372 cannot generate the 1 Hz signal. |
| `disable1Hz`   | none                                            | Disables 1 Hz output.                                        |
| `setAlarm`     | `int` `seconds`, [`minutes`[, `hour` [,`day`]]] | Set an alarm time, where one either can specify, only seconds, seconds and minutes, seconds, minutes, and hour, or seconds, minutes, hour, and day of the month. Returns `false` if the RTC is not able to set an alarm, otherwise `true`. So far, only DS1307 does not have an alarm. |
| `enableAlarm`  | none                                            | Enable the alarm interrupt. Returns `false` if no alarm is supported by the RTC, otherwise `true`. |
| `disableAlarm` | none                                            | Disables alarm.                                              |
| `setOffset`    | `int` `value` for 0.1 ppm correction steps      | Note that not all RTCs support trimming and they have different step sizes, ranging from 0.1 ppm to 4.3 ppm. The function will try to approximate the passed value as much as possible. Returns `true` when correction can be applied, `false` when the RTC does not have an offset register. |
| `getRegister`  | `byte` `register address`                       | Returns contents of RTC register.                            |
| `setRegister`  | `byte` `register address,` `byte` `value`       |                                                              |

