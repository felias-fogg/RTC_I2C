// Simple test for RTC_I2C
// Just include one RTC class and try out all implemented methods

#include <RTC_PCF8563.h>
#include <Wire.h>

#define PIN1HZ 2
#define PIN32KHZ 3
#define PINALARM 2

PCF8563 rtc;

const char *monthAbbr[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


void setup() {

  pinMode(PIN1HZ, INPUT_PULLUP);
  pinMode(PIN32KHZ, INPUT_PULLUP);
  pinMode(PINALARM, INPUT_PULLUP);

  Serial.begin(115200);
  while (!Serial); // wait for Arduino Serial Monitor
  Serial.println();
  Serial.println(F("I2C RTC interactive test"));
  if (!rtc.begin()) {
    Serial.println(F("RTC not present!"));
    while (1);
  }
  help();
}

void loop() {
  char c = '\0';
  tm timeParts;
  int temp;
  bool alarm;
  int offset;
  int reg;
  int val;
  unsigned long pw;

  while (c <= ' ') {
    while (!Serial.available());
    c = Serial.read();
  }
  switch (c) {
  case '?':
    help();
    break;
  case 's':
    initRTC();
    break;
  case 'c':
    rtc.getTime(timeParts);
    showTime(timeParts);
    if (!rtc.isValid()) Serial.println(F("(Time may be invalid)"));
    break;
  case 'd':
    rtc.getTime(timeParts);
    showDate(timeParts);
    if (!rtc.isValid()) Serial.println(F("(Date may be invalid)"));
    break;
  case 'f':
    rtc.getTime(timeParts);
    if (timeParts.tm_min < 1)
      timeParts.tm_min += 59;
    else {
      if (timeParts.tm_hour++ == 23) timeParts.tm_hour = 0;
      timeParts.tm_min -= 1;
    }
    rtc.setTime(timeParts);
    Serial.print(F("Time has advanced by 59 minutes: "));
    rtc.getTime(timeParts);
    showTime(timeParts);
    break;
  case 'y':
    timeParts = {};
    timeParts.tm_sec = 0;
    timeParts.tm_min = 59;
    timeParts.tm_hour = 23;
    timeParts.tm_mday = 28;
    timeParts.tm_mon = 1;
    timeParts.tm_year = 2100 - 1900;
    timeParts.tm_wday = 0; // Sunday
    rtc.setTime(timeParts);
    Serial.println(F("Time has advanced to 28.2.2100, 23:59:00"));
    break;
  case '1':
    if ((rtc.getCapabilities() & RTC_CAP_1HZ) == 0) {
      unsupported();
      break;
    }
    rtc.enable1Hz();
    Serial.println(F("1 Hz signal enabled"));
    break;
  case '3':
    if ((rtc.getCapabilities() & RTC_CAP_32KHZ) == 0) {
      unsupported();
      break;
    }
    rtc.enable32kHz();
    Serial.println(F("32 kHz signal enabled"));
    break;
  case '0':
    rtc.disable1Hz();
    rtc.disable32kHz();
    Serial.println(F("All square wave signals disabled"));
    break;
  case 'i':
    Serial.print(F("32kHz output: "));
    pw = pulseIn(PIN32KHZ, LOW, 10000);
    pw += pulseIn(PIN32KHZ, HIGH, 10000);
    if (pw == 0)
      Serial.print(0);
    else
      Serial.print(1000000UL / pw);
    Serial.print(F(" Hz\n\r1Hz Output: "));
    pw = pulseIn(PIN1HZ, LOW, 2000000);
    if (pw != 0) pw += pulseIn(PIN1HZ, HIGH, 2000000);
    // Serial.print(pw);Serial.print(' ');
    if (pw < 1100000 && pw > 900000)
      Serial.println(F("1 Hz"));
    else if (pw == 0)
      Serial.println(F("0 Hz"));
    else {
      Serial.print(pw);
      Serial.println(F(" us pulse width"));
    }
    Serial.print(F("INT pin: "));
    Serial.println(digitalRead(PINALARM));
    break;
  case 'h':
    if ((rtc.getCapabilities() & RTC_CAP_ALARM) == 0) {
      unsupported();
      break;
    }
    rtc.disableAlarm();
    rtc.getTime(timeParts);
    rtc.setAlarm(timeParts.tm_min, (timeParts.tm_hour + 1) % 24);
    rtc.enableAlarm();
    Serial.print(F("Alarm set to hour/minute "));
    Serial.print((timeParts.tm_hour + 1) % 24);
    Serial.print(':');
    Serial.println(timeParts.tm_min);
    break;
  case 'm':
    if ((rtc.getCapabilities() & RTC_CAP_HOURLY_ALARM) == 0) {
      unsupported();
      break;
    }
    rtc.getTime(timeParts);
    rtc.disableAlarm();
    rtc.setAlarm(timeParts.tm_min);
    rtc.enableAlarm();
    Serial.print(F("Alarm set to minute "));
    Serial.println(timeParts.tm_min);
    break;
  case 'n':
    if ((rtc.getCapabilities() & RTC_CAP_ALARM) == 0) {
      unsupported();
      break;
    }
    rtc.disableAlarm();
    break;
  case 'a':
    if ((rtc.getCapabilities() & RTC_CAP_ALARM) == 0) {
      unsupported();
      break;
    }
    alarm = rtc.senseAlarm();
    Serial.print(F("Alarm: "));
    Serial.println(alarm);
    if (alarm) {
      rtc.clearAlarm();
    }
    break;
  case 'A':
    rtc.disableAlarm();
    Serial.println(F("All alarms disabled"));
    break;
  case 't':
    if ((rtc.getCapabilities() & RTC_CAP_TEMP) == 0) {
      unsupported();
      break;
    }
    temp = rtc.getTemp();
    Serial.print(F("Temp: "));
    Serial.println(temp);
    break;
  case 'o':
    if ((rtc.getCapabilities() & RTC_CAP_OFFSET) == 0) {
      unsupported();
      break;
    }
    offset = Serial.parseInt();
    rtc.setOffset(offset);
    Serial.print(F("Offset reg: "));
    Serial.println(offset);
    break;
  case 'r':
    reg = parse2Hex();
    if (reg < 0) {
      Serial.println(F("2 digit hex number expected"));
      while (Serial.available()) Serial.read();
      break;
    }
    Serial.print(F("Register 0x"));
    Serial.print(reg, HEX);
    Serial.print(F("="));
    Serial.print(rtc.getRegister(reg), BIN);
    Serial.print(F(" (0x"));
    Serial.print(rtc.getRegister(reg), HEX);
    Serial.println(F(")"));
    break;
  case 'R':
    reg = parse2Hex();
    if (reg < 0) {
      Serial.println(F("2 digit hex number expected"));
      while (Serial.available()) Serial.read();
      break;
    }
    while (!Serial.available());
    if (Serial.read() != '=') {
      Serial.println(F("= expected"));
      while (Serial.available()) Serial.read();
      break;
    }
    val = parse2Hex();
    if (val < 0) {
      Serial.println(F("2 digit hex number expected"));
      while (Serial.available()) Serial.read();
      break;
    }
    rtc.setRegister(reg, val);
    Serial.println(F("Register set"));
    break;
  default:
    Serial.print(F("Unknown command '"));
    Serial.print(c);
    Serial.println(F("'"));
    break;
  }
  while (Serial.available() && Serial.peek() <= ' ') Serial.read();
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

void unsupported() {
  Serial.println(F("Command is unsupported on this RTC"));
}

void help() {
  Serial.println(F("\n\rPossible commands:\n\r"
                   "  ?      - help\n\r"
                   "  s      - init clock & set clock to compile time\n\r"
                   "  c      - show time\n\r"
                   "  d      - show date\n\r"
                   "  1      - activate 1 Hz signal\n\r"
                   "  3      - activate 32 kHz signal\n\r"
                   "  0      - disable all square wave signals\r\n"
                   "  i      - input on all possible pins and measure pule width\n\r"
                   "  h      - set minute/hour alarm to 1h in the future\n\r"
                   "  m      - set hourly alarm matching the current minute\n\r"
                   "  a      - sense alarm and clear flag\n\r"
                   "  A      - disable alarms\n\r"
                   "  n      - disable alarm\n\r"
                   "  f      - skip 59 minutes forward\n\r"
                   "  y      - skip to 23:59:00 of February 28, 2100\n\r"
                   "  o<num> - set offset register\n\r"
                   "  t      - read out temperature\n\r"
                   "  rXX    - show register with hex address XX\n\r"
                   "  RXX=YY - set register XX with hex YY"));
}

bool initRTC() {
  tm new_tm;
  bool config = false;
  bool valid = false;

  Serial.print(F("Compilation Time: "));
  Serial.print(__DATE__);
  Serial.print(' ');
  Serial.println(__TIME__);

  tm set_time = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  bool parse = parseDate(__DATE__, set_time) && parseTime(__TIME__, set_time);
  // to fill in the weekday and the day of year correctly, round trip through mktime
  time_t assembled_time = mktime(&set_time);
  set_time = *localtime(&assembled_time);
  printTmComponents(set_time, Serial);

  bool success = rtc.init(BatteryMode::DIRECT_SWITCHING);
  if (parse) {
    rtc.setTime(set_time);
    rtc.getTime(new_tm);

    valid = rtc.isValid();
    if (valid && TimeUtils::sameTime(set_time, new_tm)) {
      config = true;
    }
  }
  // Serial.println(rtc.isValid());
  if (parse && config) {
    Serial.print("RTC configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
    success = true;
  } else if (parse) {
    Serial.print("RTC Communication Error:\n\rInput=   ");
    printTmComponents(set_time, Serial);
    Serial.print(F("Response="));
    printTmComponents(new_tm, Serial);
    Serial.print(F("Valid=   "));
    Serial.println(valid);
    success = false;
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
    success = false;
  }
  return success;
}

int parse2Hex() {
  char c;
  int res = 0;
  while (!Serial.available());
  c = Serial.read();
  res = checkHex(c);
  if (res < 0) return res;
  while (!Serial.available());
  c = Serial.read();
  res = checkHex(c) | (res << 4);
  return res;
}

int checkHex(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (toupper(c) >= 'A' && toupper(c) <= 'F')
    return toupper(c) - 'A' + 10;
  else
    return -1;
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

bool parseDate(const char *str, tm &timeParts) {
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthAbbr[monthIndex]) == 0) break;
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
