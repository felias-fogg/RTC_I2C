# RTC I2C Examples<!--! {#page_the_examples} -->

These example programs demonstrate how to use the RTC I2C library.

___

<!--! @if GITHUB -->

- [RTC I2C Examples](#rtc-i2c-examples)
  - [Simple](#simple)
  - [Iterate](#iterate)
  - [Test All](#test-all)

<!--! @endif -->

<!--! @tableofcontents -->

<!--! @m_footernavigation -->

## Simple<!--! {#examples_simple} -->

This example demonstrates using the basic set and get methods of an RTC device.
It shows how to initialize an RTC, set the time, and read back the time to verify the values were stored correctly.

- [The simple example on GitHub](https://github.com/EnviroDIY/RTC_I2C/tree/master/examples/simple)

<!--! @subpage example_simple -->

## Iterate<!--! {#examples_iterate} -->

This example demonstrates testing multiple RTC modules simultaneously.
It includes a large array of different RTC device types and iterates through them, printing the time from each connected device.
This is useful for testing hardware compatibility and verifying that multiple RTC modules can be used on the same I2C bus.

- [The iterate example on GitHub](https://github.com/EnviroDIY/RTC_I2C/tree/master/examples/iterate)

<!--! @subpage example_iterate -->

## Test All<!--! {#examples_testall} -->

This example provides an interactive test of all implemented RTC methods.
It includes a menu-driven interface for testing various functions such as reading and writing time, configuring alarms, and accessing interrupt pins.
This example is useful for comprehensive testing and development of RTC functionality.

- [The testall example on GitHub](https://github.com/EnviroDIY/RTC_I2C/tree/master/examples/testall)

<!--! @subpage example_testall -->
