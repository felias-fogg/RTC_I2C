// Test sketch that iterates over a large set of RTCs and prints the time.

#include <RTC_I2C.h>
#include <RTC_DS1307.h>
#include <RTC_DS1337.h>
#include <RTC_DS3231.h>
#include <RTC_MCP79410.h>
#include <RTC_PCF8523.h>
#include <RTC_PCF8563.h>
#include <RTC_RS5C372.h>
#include <RTC_RV3028.h>
#include <RTC_RV3028U.h>
#include <RTC_RV3032.h>
#include <RTC_RV8523.h>
#include <RTC_RV8803.h>
#include <RTC_SD2405.h>

#define MAXRTC 13

DS1307 rtc0;
DS1337 rtc1;
DS3231 rtc2;
MCP79410 rtc3;
PCF8523 rtc4;
PCF8563 rtc5;
RS5C372 rtc6;
RV3028 rtc7;
RV3028U rtc8;
RV3032 rtc9;
RV8523 rtc10;
RV8803 rtc11;
SD2405 rtc12;

RTC *rtc[MAXRTC] = { &rtc0, &rtc1, &rtc2, &rtc3, &rtc4, &rtc5, &rtc6, &rtc7, &rtc8, &rtc9, &rtc10, &rtc11, &rtc12};


void setup(void) {
  for (byte i=0; i < MAXRTC; i++) rtc[i]->begin();
}

void loop(void) {
  tmElements_t tm;
  for (byte i=0; i < MAXRTC; i++) {
    Serial.print(F("RTC"));
    Serial.print(i);
    Serial.print(F(": "));
    rtc[i]->getTime(tm);
    showTime(tm);
    Serial.print(F("   "));
    showDate(tm);
    Serial.println();
  }
  delay(1000);
}


void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void showTime(tmElements_t tm) {
  print2digits(tm.Hour);
  Serial.write(':');
  print2digits(tm.Minute);
  Serial.write(':');
  print2digits(tm.Second);
}


void showDate(tmElements_t tm) {
  Serial.print(tm.Day);
  Serial.write('.');
  Serial.print(tm.Month);
  Serial.write('.');
  Serial.print(1970+tm.Year);
}
