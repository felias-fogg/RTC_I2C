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

The interface is minimal, but is just enough to get the basic functionality. And it is provided by a base class. This means you can even use many RTCs in parallel without caring about what particular model you use. The supported features are sort of a least common denominator. However, I wanted to have some kind of alarm, a 1 Hz output, and being able to make use of an offset register. Unfortunately, one RTC does not have alarms at all, another RTC does not support the output of a 1 Hz signal, and two RTCs do not have an offset register. In order to check at runtime what capabilities an RTC has, the method `getCapabilities()` will be useful (see below).

In addition, one can, of course, extend each particular RTC class with methods catering for the particular RTC model, if one wishes to do so.

This library uses Paul Stoffregen's [*Time*](https://github.com/PaulStoffregen/Time) library.

## Methods of the RTC class

| Name           | Parameters                                      | Comments                                                     |
| -------------- | ----------------------------------------------- | ------------------------------------------------------------ |
| `begin`        | none                             | Needs to be called in the beginning. Returns `false` if unsuccessful, otherwise `true`. |
| `init`        | none                                             | Restores state of clock to configuration after power-up. It sets 24 hout mode, disables all clock outputs and alarms, and it will also start the clock (if not already running) and clear all invalid flags. |
| `isValid`      | none                                            | Returns `true` if the clock is valid and running, otherwise `false`. |
| `setTime`      | Unix `time_t` value                           | Sets the RTC time from provided parameter. Will use the 24 hour format. |
| `setTime` | `tmElements_t` record | Sets the RTC time from a `tmElements_t` structure. Note that it is Unix epoch based, i.e., the year field is the number of years since 1970. |
| `getTime`      | none                                            | Returns the current time of the RTC as a Unix `time_t` value. |
| `getTime` | `&tmElements_t` record | Sets all fields of the record from the current time of the RTC. |
| `enable32kHz`  | none                                            | Enables output of clock pulses. Currently, all RTCs support that.                            |
| `disable32kHz` | none                                            | Disables output of clock pulses.                             |
| `enable1Hz`    | none                                            | Enables output of 1 Hz signal.  Currently, only RC5C372 cannot generate the 1 Hz signal. |
| `disable1Hz`   | none                                            | Disables 1 Hz output.                                        |
| `setAlarm`     | `byte` minute [, `byte` hour ]| Set an alarm time, where one either can specify only  minutes or minutes and hour. If the RTC has more than one alarm, the first one is used. Currently only DS1307 does not support alarms.|
| `enableAlarm`  | none                                            | Enable the alarm interrupt. |
| `disableAlarm` | none                                            | Disables alarm.                                              |
| `senseAlarm`   | none                                            | Returns `true`if alarm has been raised. |
| `clearAlarm`   | none                                            | Clear alarm flag.|
| `setOffset`    | `int` value for 0.01 ppm correction steps and one optional `byte` mode parameter | Note that not all RTCs support trimming and they have different step sizes, ranging from 0.1 ppm to 4.3 ppm. The function will try to approximate the passed value as much as possible. Currently, only DS1307, DS1337, and PCF8563 do not have an offset register. |
| `getRegister`  | `byte` register address                       | Returns contents of RTC register.                            |
| `setRegister`  | `byte` register address, `byte` value | Sets RTC register to value.                                                          |
| `getCapabilities` | none | Returns a byte with capability bits: Bit 0 = can output 32kHz signal, bit 1 = can output 1 Hz signal, bit 2 = has alarm, bit 3 = has offset register  |

## Caveats

While this library is a great tool when you want to use different RTCs in parallel (there are not too many use cases for this, though, I guess), you should be aware that the RTCs often have similar I2C addresses. In order to avoid I2C address clashes, you need an I2C multiplexer or have to use different I2C buses. The former can be accomplished by a I2C multiplexer such as TCA9548A or PCA9548. The latter could be achieved by using different Wire-alike software I2C instances, such as *SlowSoftWire* instances. However, this implies that you declare all RTC classes to be template classes. I tried that out and stopped doing it because one would have to put all header and implementation files together. So, currently, the multiplexer is the only viable option when you want to run RTCs with the same I2C address in parallel.

If you want to run just one RTC, the above is, of course, irrelevant. And if the functionality of the API is enough for you, you just found the right tool. However, as mentioned above, you can add all the features of your favorite RTC.  

Finally, it should be pointed out that this library seems to be more memory hungry than [RTClib](https://github.com/adafruit/RTClib). This is probably caused by the dynamic binding of methods. 

## Example sketches

There are the following example sketches:

`simple.ino`: Simple test sketch for setting and getting time. 

`testall.ino`: Interactive sketch that can be used to test all functions of an RTC. Simply change include file and class name. If you want to monitor the outputs (SQW, INT), you also need to define the constants `PIN1HZ`, `PIN32KHZ` and `PINALARM`.

