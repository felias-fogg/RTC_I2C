// Simple test for I2CRTC
// Just include one RTC class and try it out

#include <RTCDS1307.h>
#include <Wire.h>

RTCDS1307 RTC;

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
tmElements_t tm;


void setup(void) {
  bool parse=false;
  bool config=false;

  Serial.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println();
  Serial.println(F("I2CRTC simple test")); 
  if (!RTC.begin()) {
    Serial.println(F("RTC not present!"));
    while (1);
  }
  RTC.init();
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    if (RTC.setTime(tm)) {
      config = true;
    }
  }

  delay(200);
  if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }
  Serial.println(F("Waiting 5 seconds ..."));
  delay(5000);

  RTC.getTime(tm);
  print2digits(tm.Hour);
  Serial.write(':');
  print2digits(tm.Minute);
  Serial.write(':');
  print2digits(tm.Second);
  Serial.print("  ");
  Serial.print(tm.Day);
  Serial.write('.');
  Serial.print(tm.Month);
  Serial.write('.');
  Serial.print(1970+tm.Year);
  Serial.println();
}

void loop() {
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

