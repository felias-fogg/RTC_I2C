#include <RTC_MCP79410.h>

void MCP79410::init( __attribute__ ((unused)) byte mode) {
  setRegister(MCP79410_CONTROL, 0x80);
}

bool MCP79410::isValid(void) {
  return ((getRegister(MCP79410_STATUS) & 0b10000) == 0); // osciallator is running
}

void MCP79410::setAlarm(byte minute, byte hour) {
  tmElements_t tm;
  time_t t;
  getTime(tm); // current time
  if (!((tm.Minute < minute && tm.Hour == hour) || (tm.Hour < hour))) { // alarm should be next day
    t = makeTime(tm)+SECS_PER_DAY;
    breakTime(t, tm);
  }
  setRegister(MCP79410_ALARM, bin2bcd(0)); // set second alarm
  setRegister(MCP79410_ALARM+1, bin2bcd(minute)); // set second alarm
  setRegister(MCP79410_ALARM+2, bin2bcd(hour)); // set hour alarm
  setRegister(MCP79410_ALARM+3, 0x70 | bin2bcd(tm.Wday)); // set weekday alarm and set match condition
  setRegister(MCP79410_ALARM+4, bin2bcd(tm.Day)); // set day of month
  setRegister(MCP79410_ALARM+5, bin2bcd(tm.Month)); // set day of month
}


void MCP79410::enableAlarm(void) { 
  byte ctr = getRegister(MCP79410_CONTROL);
  setRegister(MCP79410_CONTROL, ctr | 0b10000); // set the ALM0 bit
}

void MCP79410::disableAlarm(void) {
  byte ctr = getRegister(MCP79410_CONTROL+1);
  setRegister(MCP79410_CONTROL, (ctr & 0b11101111));  // clear ALM0 bit
}

bool MCP79410::senseAlarm(void) {
  return ((getRegister(MCP79410_ALARM+3) & 0b1000) != 0);
}

void MCP79410::clearAlarm(void) {
  byte ctr = getRegister(MCP79410_ALARM+3);
  setRegister(MCP79410_ALARM+3, (ctr & 0b11110111)); 
}


void MCP79410::enable32kHz(void) {
  setRegister(MCP79410_CONTROL, (getRegister(MCP79410_CONTROL) & 0b10111100) | 0b1000011); // enable SQW 32 kHz
}

void MCP79410::disable32kHz(void) {
  setRegister(MCP79410_CONTROL, getRegister(MCP79410_CONTROL) & ~0b01000000); // disable SQW
}

void MCP79410::enable1Hz(void) {
  setRegister(MCP79410_CONTROL, (getRegister(MCP79410_CONTROL) & 0b10111000) | 0b1000000); // set 1Hz
}

void MCP79410::disable1Hz(void) {
  disable32kHz();
}



// negative values make the clock faster by roughly 1 ppm/LSB in mode 0.
// The range of the internal parameter goes from -128 to +127, but they use
// apparently sign + magnitude instead of 2ers complement.
// I do not support the coarse claibration, because it will screw up
// the SQW output and is too coarse anyways.
void MCP79410::setOffset(int offset, byte mode) {
  bool sign = false;
  if (mode == 2) {
    setRegister(MCP79410_OFFSET, offset&0xFF);
  } else {
    if (offset < 0) {
      sign = true;
      offset = -offset;
    }
    offset = (offset+50)/100;
    if (offset < -127) offset = -127;
    else if (offset > 127) offset = 127;
    //Serial.println(sign);
    //Serial.println(offset);
    setRegister(MCP79410_OFFSET, (sign<<7)|offset);
  }
  setRegister(MCP79410_CONTROL, getRegister(MCP79410_CONTROL) & 0b11111011); // clear RS2
}

  
