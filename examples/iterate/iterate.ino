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

RTC_I2C *rtc[] = {
  // new DS1307(),   // Analog Devices DS1307, 0x68
  // new DS1337(),   // Analog Devices DS1337, 0x68
  new DS3231(), // Maxim Integrated DS3231, 0x68
  // new MCP79410(), // Microchip MCP79410, 0x6F
  // new PCF8523(),  // NXP PCF8523, 0x68
  // new PCF8563(),  // NXP PCF8563, 0x51
  // new RS5C372(),  // Ricoh RS5C372, 0x32
  // new RV3028(),   // Micro Crystal RV3028, 0x52
  // new RV3028U(),  // Micro Crystal RV3028U, 0x52
  new RV3032(), // Micro Crystal RV3032, 0x51
  // new RV8523(),   // Micro Crystal RV8523, 0x68
  new RV8803(), // Micro Crystal RV8803, 0x32
  // new SD2405()    // Epson/DFRobot SD2405, 0x32
};
uint8_t n_rtc = sizeof(rtc) / sizeof(rtc[0]);
bool connected[sizeof(rtc) / sizeof(rtc[0])] = {false};
uint8_t n_attached = 0;


void setup() {
  Serial.begin(115200);
  Serial.println(F("Starting RTC iteration test...\n"));

  Serial.print(F("Compilation Time: "));
  Serial.print(__DATE__);
  Serial.print(' ');
  Serial.println(__TIME__);

  tm set_time = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  bool use_compile_time = parseDate(__DATE__, set_time) && parseTime(__TIME__, set_time);
  // to fill in the weekday and the day of year correctly, round trip through mktime
  time_t assembled_time = mktime(&set_time);
  set_time = *localtime(&assembled_time);
  printTmComponents(set_time, Serial);

  if (!use_compile_time) {
    Serial.println(F("Using default time because compile time could not be parsed."));
    // Friday, September 25, 2026 at 7:05:00 PM UTC in Unix epoch time
    timestamp_t ts = 1790363100;
    set_time = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    // convert to a tm object
    TimeUtils::fillTimeParts(ts, 0, epochStart::unix_epoch, set_time);
    printTmComponents(set_time, Serial);
  }

  for (byte i = 0; i < n_rtc; i++) {
    Serial.print(F("Initializing RTC"));
    Serial.print(i);
    Serial.print(F(": "));
    Serial.print(rtc[i]->getMakeModel());
    Serial.print(F(" at "));
    Serial.print(String(rtc[i]->getAddress(), HEX));
    Serial.println();

    bool success = beginRTC(*rtc[i], set_time);

    Serial.print(F("    ..."));
    Serial.println(success ? "success" : "failure");
    if (!success) {
      connected[i] = false;
    } else {
      connected[i] = true;
    }
  }
}

void loop() {
  Serial.println("\n---\n");
  for (byte i = 0; i < n_rtc; i++) {
    if (!connected[i]) {
      Serial.print(F("Skipped RTC"));
      Serial.print(i);
      Serial.print(F(": "));
      Serial.print(rtc[i]->getMakeModel());
      Serial.print(F(" at "));
      Serial.print(String(rtc[i]->getAddress(), HEX));
      Serial.println(F(" because setup failed"));
      continue;
    }
    Serial.print(F("RTC"));
    Serial.print(i);
    Serial.print(F(": "));
    Serial.print(rtc[i]->getMakeModel());
    Serial.print(F(" at 0x"));
    Serial.print(String(rtc[i]->getAddress(), HEX));
    Serial.print(F(": "));
    tm timeParts = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    rtc[i]->getTime(timeParts);
    showDate(timeParts);
    Serial.print(F(" "));
    showTime(timeParts);
    Serial.println();
    Serial.print(F("  asctime: "));
    Serial.println(asctime(&timeParts));
    printTmComponents(timeParts, Serial);
    Serial.println();
  }
  delay(5000L);
}

const char *months[] = {"January", "February", "March",     "April",   "May",      "June",
                        "July",    "August",   "September", "October", "November", "December"};
const char *week_days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void printTmComponents(const tm &timeStruct, Stream &stream) {
  stream.print("    Year: ");
  stream.print(timeStruct.tm_year + 1900);
  stream.print(", Month: ");
  stream.print(timeStruct.tm_mon + 1);
  stream.print(" (");
  stream.print(timeStruct.tm_mon >= 0 && timeStruct.tm_mon < 12 ? months[timeStruct.tm_mon] : "invalid");
  stream.print(")");
  stream.print(", Day: ");
  stream.println(timeStruct.tm_mday);
  stream.print("    Day of Year: ");
  stream.print(timeStruct.tm_yday + 1);
  stream.print(", Day of Week: ");
  stream.print(timeStruct.tm_wday + 1);
  stream.print(" (");
  stream.print((timeStruct.tm_wday >= 0 && timeStruct.tm_wday < 7) ? week_days[timeStruct.tm_wday] : "invalid");
  stream.println(")");
  stream.print("    Hour: ");
  stream.print(timeStruct.tm_hour);
  stream.print(", Minute: ");
  stream.print(timeStruct.tm_min);
  stream.print(", Second: ");
  stream.println(timeStruct.tm_sec);
  stream.print("    DST Flag: ");
  stream.print(timeStruct.tm_isdst);
#ifdef __TM_GMTOFF
  stream.print("GMT Offset: ");
  stream.print(timeStruct.__TM_GMTOFF);
#endif
#ifdef __TM_ZONE
  stream.print("Time Zone: ");
  stream.print(timeStruct.__TM_ZONE);
#endif
  stream.println();
}

bool beginRTC(RTC_I2C &rtc, tm &set_time) {
  tm new_tm;
  bool config = false;
  bool valid = false;

  bool success = rtc.begin(&Wire, BatteryMode::DIRECT_SWITCHING);
  if (!success) {
    return success;
  }

  Serial.print(F("  RTC Set Time: "));
  rtc.setTime(set_time);
  printTmComponents(set_time, Serial);
  Serial.print(F("  RTC Returned Time: "));
  rtc.getTime(new_tm);
  printTmComponents(new_tm, Serial);

  valid = rtc.isValid();
  if (valid && TimeUtils::sameTime(set_time, new_tm)) {
    config = true;
  }
  // Serial.println(rtc.isValid());
  if (config) {
    Serial.print(F("  RTC Time Valid and Matches Compilation Time\n\r"));
    success = true;
  } else {
    Serial.print("RTC Communication Error:\n\rInput=   ");
    printTmComponents(set_time, Serial);
    Serial.print(F("Response="));
    printTmComponents(new_tm, Serial);
    Serial.print(F("Valid=   "));
    Serial.println(valid);
    success = false;
  }
  return success;
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

bool parseTime(const char *str, tm &timeParts) {
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  timeParts.tm_hour = Hour;
  timeParts.tm_min = Min;
  timeParts.tm_sec = Sec;
  return true;
}

const char *monthAbbr[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

bool parseDate(const char *str, tm &timeParts) {
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthAbbr[monthIndex]) == 0) {
      break;
    }
  }
  if (monthIndex >= 12) return false;
  timeParts.tm_mday = Day;
  timeParts.tm_mon = monthIndex;
  timeParts.tm_year = Year - 1900;
  return true;
}

void showTime(tm timeParts) {
  print2digits(timeParts.tm_hour);
  Serial.write(':');
  print2digits(timeParts.tm_min);
  Serial.write(':');
  print2digits(timeParts.tm_sec);
}

void showDate(tm timeParts) {
  Serial.print((timeParts.tm_wday >= 0 && timeParts.tm_wday < 7) ? week_days[timeParts.tm_wday] : "invalid");
  Serial.write(' ');
  Serial.print(timeParts.tm_mon >= 0 && timeParts.tm_mon < 12 ? months[timeParts.tm_mon] : "invalid");
  Serial.write(' ');
  Serial.print(timeParts.tm_mday);
  Serial.print(F(", "));
  Serial.print(1900 + timeParts.tm_year);
}
