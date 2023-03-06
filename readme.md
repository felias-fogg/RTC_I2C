# RTC_I2C



## A universal API for I2C RTCs

This library provides a universal API to a number of I2C RTCs, which are described in one of my [blog articles](https://hinterm-ziel.de/index.php/2023/02/24/everything-you-wanted-to-know-about-real-time-clocks-but-were-afraid-to-ask/):

* DS1307,
* DS1337,
* DS3231,
* MCP79410, 
* PCF8523,
* PCF8563,
* RS5C372,
* RV-3028, 
* RV-3032, 
* RV-8523,
* RV-8803, and
* SD2405. 

The interface is minimal, but is just enough to get the basic functionality. And it is provided by a base class. This means you can even use many RTCs in parallel without caring about what particular model you use. I tried to identify the least common denominator of the functionality. However, I wanted to have some kind of alarm, and being able to make use of an offset register and read the temperature, if possible. Unfortunately, one RTC does not have alarms at all,  one RTC does not allow recurring alarms, a few RTCs do not have an offset register, and many do not posses the ability to read out the temperature. 

In order to check at runtime what capabilities an RTC has, the method `getCapabilities()` will be useful (see table entry below). The list of capabilities and at which pins you can sense the different signals appears in a table in a section further down. 

In addition, one can, of course, extend each particular RTC class with methods catering for the particular RTC model, if one wishes to do so.

This library uses Paul Stoffregen's [*Time*](https://github.com/PaulStoffregen/Time) library.

## Methods of the RTC class

| Name           | Parameters                                      | Comments                                                     |
| -------------- | ----------------------------------------------- | ------------------------------------------------------------ |
| `begin`        | none                             | Needs to be called in the beginning in order to initialize the I2C bus. It is also checked whether the RTC can be addressed on the I2C bus. Returns `false` if unsuccessful, otherwise `true`. |
| `init`        | optional `byte` switch-mode  (default 1) | Restores state of clock to configuration after power-up. It sets 24 hour mode, disables all clock outputs and alarms, and it will also start the clock (if not already running) and clear all invalid flags. If switch mode = 0, then no switching from regular power supply to backup is enabled, if =1 , then level switching is enabled, if =2 then direct switching is enabled. The latter should only be used when Vcc and Vbat are significantly different. |
| `isValid`      | none                                            | Returns `true` if the clock is valid and running, otherwise `false`. |
| `setTime`      | Unix `time_t` value                           | Sets the RTC time from provided parameter. Will use the 24 hour format. |
| `setTime` | `tmElements_t` record | Sets the RTC time from a `tmElements_t` structure. Note that it is Unix epoch based, i.e., the year field is the number of years since 1970. |
| `getTime`      | optional parameter `bool` blocking (default `false`) | Returns the current time of the RTC as a Unix `time_t` value. If blocking=`true`, then the method waits until the next second has just started. |
| `getTime` | `&tmElements_t` record, optional parameter `bool` blocking (default `false`) | Sets all fields of the record from the current time of the RTC. For blocking see above. |
| `enable32kHz`  | none                                            | Enables output of clock pulses. Currently, all RTCs support that.                            |
| `disable32kHz` | none                                            | Disables output of clock pulses.                             |
| `enable1Hz`    | none                                            | Enables output of 1 Hz signal.  Currently, only RC5C372 cannot generate the 1 Hz signal. |
| `disable1Hz`   | none                                            | Disables 1 Hz output.                                        |
| `setAlarm`     | `byte` minute, `byte` hour | Set an alarm time, where one has to specify minute and hour. If the RTC has more than one alarm, the first one is used. Only DS1307 and RV-3028 using the Unix clock do not support alarms. The MCP79140 does not support recurring alarms, i.e., one has to call the setAlarm method again. |
| `enableAlarm`  | none                                            | Enables the alarm interrupt. |
| `disableAlarm` | none                                            | Disables alarm.                                              |
| `senseAlarm`   | none                                            | Returns `true`if alarm has been raised. |
| `clearAlarm`   | none                                            | Clears alarm flag. |
| `setOffset`    | `int` value for 0.01 ppm correction steps and one optional `byte` mode parameter | Note that not all RTCs support trimming and they have different step sizes, ranging from 0.1 ppm to 4.3 ppm. The function will try to approximate the passed value as much as possible. Only DS1307, DS1337, and PCF8563 do not have an offset register. PCF8523 and RV-8523 offer different modes (default mode is 1). With mode 2, one can set the offset register in the RTC native representation. |
| `getTemp` | none | Returns temperature as an integer value, if the RTC has a user accessible temperature sensor. |
| `getRegister`  | `byte` register address                       | Returns contents of RTC register.                            |
| `setRegister`  | `byte` register address, `byte` value | Sets RTC register to value.                                                          |
| `getCapabilities` | none | Returns a byte with capability bits: Bit 0 = can output 32kHz signal, bit 1 = can output 1 Hz signal, bit 2 = has alarm, bit 3 = has offset register, 4 = has temperature sensor, 5 = uses strange register addressing scheme |

## Caveats

While this library is a great tool when you want to use different RTCs in parallel (there are not too many use cases for this, though, I guess), you should be aware that the RTCs often have similar I2C addresses. In order to avoid I2C address clashes, you need an I2C multiplexer or have to use different I2C buses. The former can be accomplished by a I2C multiplexer such as TCA9548A or PCA9548. The latter could be achieved by using different Wire-alike software I2C instances, such as *SlowSoftWire* instances. However, this implies that you declare all RTC classes to be template classes. I tried that out and stopped doing it because one would have to put all header and implementation files together. So, currently, the multiplexer is the only viable option when you want to run RTCs with the same I2C address in parallel.

If you want to run just one RTC, the above is, of course, irrelevant. And if the functionality of the API is enough for you, you just found the right tool. However, as mentioned above, you can add all the features of your favorite RTC.  

Finally, it should be pointed out that this library seems to be more memory hungry than [RTClib](https://github.com/adafruit/RTClib). This is probably caused by the dynamic binding of methods. 

## Example sketches

There are the following example sketches:

`simple.ino`: Simple test sketch for setting and getting time. 

`testall.ino`: Interactive sketch that can be used to test all functions of an RTC. Simply change include file and class name. If you want to monitor the outputs (SQW, INT), you also need to define the constants `PIN1HZ`, `PIN32KHZ` and `PINALARM`. Which outputs to use can be seen in the next section.

## RTC capabilities

The following table lists all the capabilities for each RTC. For the frequency outputs and alarms, it also states at which pins the signal can be sensed. For the offset register, the step size is mentioned. And for the temperature, the accuracy is stated. Any empty entry means that the RTC does not posses this capability. Finally in the last column, we specify the maximal I2C frequency in MHz.

| RTC        | 1 Hz                               | 32 kHz                | Alarm               |  Offset (ppm) | Temp (°C) | I2C (MHz) |
| ---------- | ---------------------------------- | --------------------- | ------------------- | ------------: | --------: | --------: |
| DS1307     | `SQW/OUT`                          | `SQW/OUT`             |                     |               |           |       0.1 |
| DS1337     | `SQW/nINTB`                        | `SQW/nINTB`           | `SQW/nINTB`         |               |           |       0.4 |
| DS3231S(N) | `nINT/SQW`<sup>1)</sup>            | `32kHz`<sup>1)</sup>  | `nINT/SQW`          |          ≈0.1 |        ±3 |       0.4 |
| DS3231M    | `nINT/SQW`<sup>1)</sup>            | `32kHz`               | `nINT/SQW`          |          ≈0.1 |        ±3 |       0.4 |
| MCP79410   | `MFP`<sup>2)</sup>                 | `MFP`                 | `MFP`<sup>5)</sup>  |            ≈1 |           |       0.4 |
| PCF8523    | `nINT1/CLKOUT`                     | `nINT1/CLKOUT`        | `nINT1/CLKOUT`      | 4.34 or 4.069 |           |       1.0 |
| PCF8563    | `CLKOUT`                           | `CLKOUT`              | `nINT`              |               |           |       0.4 |
| RS5C372    | `nINTRA`<sup>2)</sup>              | `nINTRB`              | `nINTRB`            |         3.051 |           |       0.4 |
| RV-3028    | `nINT`<sup>1)</sup>                | `CLKOUT`              | `nINT`<sup>4)</sup> |        0.9537 |           |       0.4 |
| RV-3032    | `CLKOUT`<sup>1)</sup>              | `CLKOUT`              | `nINT`              |        0.2384 |        ±3 |       0.4 |
| RV-8523    | `nINT1/CLKOUT`                     | `nINT1/CLKOUT`        | `nINT1/CLKOUT`      | 4.34 or 4.069 |           |       1.0 |
| RV-8803    | `CLKOUT`<sup>1)</sup><sup>3)</sup> | `CLKOUT`<sup>3)</sup> | `nINT`              |        0.2384 |           |       0.4 |
| SD2405     | `nINT`                             | `nINT`                | `nINT`              |         3.051 |           |       0.4 |

<sup>1)</sup> The offset register influences the frequency directly.

<sup>2)</sup> The offset register influences the frequency. However, the adjustments are made every 10, 20, or 60 seconds. This means one would need a large set of measurements when one wants to to get the true average frequency.

<sup>3)</sup> The output cannot be disabled using software, but one has to pull-down the `CLKOE` pin. On the Sparkfun breakout board, this input is pulled to GND by a 100 kΩ resistor, i.e., one has to tie it to Vcc in order to enable CLKOUT. 

<sup>4)</sup> In case, one uses the class variant RV3028U, which uses the Unix time counter for time keeping, alarms are not supported.

<sup>5)</sup> Since the MCP79410 requires that one has too specify a full date, we will in this case set the next alarm date (either today or tomorrow). In other words, this is not a recurring alarm as in all other cases.

