// Simple test for RTC_I2C
// Just include one RTC class and try out all implemeted methods

#include <RTC_RS5C372.h>
#include <Wire.h>

#define PIN1HZ 2
#define PIN32KHZ 2
#define PINALARM 3

RS5C372 rtc;

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


void setup(void) {

  pinMode(PIN1HZ, INPUT_PULLUP);
  pinMode(PIN32KHZ, INPUT_PULLUP);
  pinMode(PINALARM, INPUT_PULLUP);
  
  Serial.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println();
  Serial.println(F("I2CRTC interactive test")); 
  if (!rtc.begin()) {
    Serial.println(F("RTC not present!"));
    while (1);
  }
  help();
}

void loop() {
  char c = '\0';
  tmElements_t tm;
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
  case 's': initRTC();
    break;
  case 'c': rtc.getTime(tm);
    showTime(tm);
    if (!rtc.isValid()) Serial.println(F("(Time may be invalid)"));
    break;
  case 'd': rtc.getTime(tm);
    showDate(tm);
    if (!rtc.isValid()) Serial.println(F("(Date may be invalid)"));
    break;
  case 'f': rtc.getTime(tm);
    if (tm.Minute < 2) tm.Minute += 58;
    else {
      if (tm.Hour++ == 23) tm.Hour = 0;
      tm.Minute -= 2;
    }
    rtc.setTime(tm);
    Serial.print(F("Time has advanced by 58 minutes: "));
    showTime(tm);
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
    pw = pulseIn(PIN32KHZ,LOW,10000);
    pw += pulseIn(PIN32KHZ,HIGH,10000);
    if (pw == 0) Serial.print(0);
    else Serial.print(1000000UL/pw);
    Serial.print(F(" Hz\n\r1Hz Output: "));
    pw = pulseIn(PIN1HZ,LOW,2000000);
    if (pw != 0) pw += pulseIn(PIN1HZ,HIGH,2000000);
    // Serial.print(pw);Serial.print(' ');
    if (pw < 1100000 && pw > 900000) Serial.println(F("1 Hz"));
    else if (pw == 0) Serial.println(F("0 Hz"));
    else {
      Serial.print(pw);
      Serial.println(F(" us pulsewidth"));
    }
    Serial.print(F("INT pin: "));
    Serial.println(digitalRead(PINALARM));
    break;
  case 'h':
    if ((rtc.getCapabilities() & RTC_CAP_ALARM) == 0) {
      unsupported();
      break;
    }
    rtc.getTime(tm);
    rtc.setAlarm(tm.Minute,(tm.Hour+1)%24);
    rtc.enableAlarm();
    Serial.print(F("Alarm set to hour/minute "));
    Serial.print((tm.Hour+1)%24);
    Serial.print(':');
    Serial.println(tm.Minute);
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
      rtc.disableAlarm();
    }
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
    Serial.print(reg,HEX);
    Serial.print(F("="));
    Serial.print(rtc.getRegister(reg),BIN);
    Serial.print(F(" (0x"));
    Serial.print(rtc.getRegister(reg),HEX);
    Serial.println(F(")"));
    break;
  case 'R':
    reg=parse2Hex();
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
    val=parse2Hex();
    if (val < 0) {
      Serial.println(F("2 digit hex number expected"));
      while (Serial.available()) Serial.read();
      break;
    }
    rtc.setRegister(reg,val);
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

void unsupported(void) {
  Serial.println(F("Command is unsupported on this RTC"));
}

void help() {
  Serial.println(F("\n\rPossible commands:\n\r"
		   "  ?      - help\n\r"
		   "  s      - init clock & set clock to compile time\n\r"
		   "  c      - show time\n\r"
		   "  f      - skip 58 minutes forward\n\r"
		   "  d      - show date\n\r"
		   "  1      - activate 1 Hz signal\n\r"
		   "  3      - activate 32 kHz signal\n\r"
		   "  0      - disable all square wave signals\r\n"
		   "  i      - input on all possible pins and measure pule width\n\r"
		   "  h      - set minute/hour alarm to 1h in the future\n\r"
		   "  a      - sense alarm and clear flag\n\r"
		   "  n      - disable alarm\n\r"
		   "  o<num> - set offset register\n\r"
		   "  t      - read out temperature\n\r"
		   "  rXX    - show register with hex address XX\n\r"
                   "  RXX=YY - set register XX with hex YY"));
}

void initRTC(void)  {
  tmElements_t tm, newtm;
  bool parse=false;
  bool config=false;
  bool valid=false;
  
  rtc.init(1); // select level switch-over mode
  if (getDate(__DATE__,tm) && getTime(__TIME__,tm)) {
    parse = true;
    rtc.setTime(tm);
    rtc.getTime(newtm);
    //Serial.println(makeTime(tm));
    //Serial.println(makeTime(newtm));
    valid = rtc.isValid();
    if (valid && makeTime(tm) == makeTime(newtm)) {
      config = true;
    }
  }
  //Serial.println(rtc.isValid());
  if (parse && config) {
    Serial.print("RTC configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.print("RTC Communication Error:\n\rInput=   ");
    Serial.println(makeTime(tm));
    Serial.print(F("Response="));
    Serial.println(makeTime(newtm));
    Serial.print(F("Valid=   "));
    Serial.println(valid);
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }
}

int parse2Hex(void) {
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
  if (c >= '0' && c <= '9') return c - '0';
  else if (toupper(c) >= 'A' && toupper(c) <= 'F') return toupper(c) - 'A' + 10;
  else return -1;
}
  

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

bool getTime(const char *str,tmElements_t &tm)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str, tmElements_t &tm)
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

void showTime(tmElements_t tm) {
  rtc.getTime(tm);
  print2digits(tm.Hour);
  Serial.write(':');
  print2digits(tm.Minute);
  Serial.write(':');
  print2digits(tm.Second);
  Serial.println();
}


void showDate(tmElements_t tm) {
  Serial.print(tm.Day);
  Serial.write('.');
  Serial.print(tm.Month);
  Serial.write('.');
  Serial.print(1970+tm.Year);
  Serial.println();
}
