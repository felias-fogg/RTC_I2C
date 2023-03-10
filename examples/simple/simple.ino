// Simple sketch for testing basic set/get methods

#include <RTC_DS3231.h>

DS3231 rtc;

void setup(void) {
  tmElements_t tm, tm1;
  Serial.begin(115200);
  while  (!Serial);
  Serial.println(F("\n\rRTC Test"));

  if (!rtc.begin()) {
    Serial.println(F("Cannot access RTC"));
    while (1);
  }
  rtc.init();
  tm = tmElements_t{1,1,1,1,2,3,55};
  rtc.setTime(tm);
  rtc.getTime(tm1);
  if (makeTime(tm) != makeTime(tm1)) {
    Serial.println(F("Not able to set RTC"));
    while (1);
  }
  delay(5100);
  rtc.getTime(tm1);
  if (makeTime(tm)+5 != makeTime(tm1)) {
    Serial.println(F("RTC does not advance"));
    while (1);
  }
  Serial.println(F("Everything in order"));
}

void loop() {
}
