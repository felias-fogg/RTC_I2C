// Simple sketch for testing basic set/get methods

#include <RTC_DS3231.h>

DS3231 rtc;

void setup() {
  tm timeParts, timeParts1;
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("\n\rRTC Test"));

  if (!rtc.begin()) {
    Serial.println(F("Cannot access RTC"));
    while (1);
  }
  rtc.init();
  timeParts = {};
  timeParts.tm_sec = 1;
  timeParts.tm_min = 1;
  timeParts.tm_hour = 1;
  timeParts.tm_mday = 2;
  timeParts.tm_mon = 2;
  timeParts.tm_year = 125;
  timeParts.tm_wday = 0;
  rtc.setTime(timeParts);
  rtc.getTime(timeParts1);
  if (!TimeUtils::sameTime(timeParts, timeParts1)) {
    Serial.println(F("Not able to set RTC"));
    while (1);
  }
  delay(5100);
  rtc.getTime(timeParts1);
  // Compare the calendar fields rather than converting through time.h.
  // RTC_I2C handles time_t/tm conversions internally with TimeUtils.
  time_t elapsed = 5;
  tm expected = timeParts;
  expected.tm_sec += elapsed;
  if (expected.tm_sec >= 60) {
    expected.tm_sec -= 60;
    ++expected.tm_min;
  }
  if (expected.tm_min >= 60) {
    expected.tm_min -= 60;
    ++expected.tm_hour;
  }
  if (expected.tm_hour >= 24) {
    expected.tm_hour -= 24;
    ++expected.tm_mday;
  }
  if (!TimeUtils::sameTime(expected, timeParts1)) {
    Serial.println(F("RTC does not advance"));
    while (1);
  }
  Serial.println(F("Everything in order"));
}

void loop() {}
