#include <RTCDS3231.h>

RTCDS3231::RTCDS3231(void) {
  _i2caddr = DS3231_ADDRESS;
  _clockreg = DS3231_CLOCKREG;
  _wdaybase = DS3231_WDAYBASE;
  _wdayfirst = DS3231_WDAYFIRST;
  _capabilities = DS3231_CAPABIL;
}

void RTCDS3231::init(void) {
  setRegister(DS3231_CONTROL, 0b00000100); // typical value after power-on, except for bit 2 (disables SQW), starts clock
  setRegister(DS3231_STATUS, 0b00000000);  // clear OSF flag, clear alarm flags, disable 32 kHz output
}

bool RTCDS3231::isValid(void) {
  return ((getRegister(DS3231_STATUS) & 0x80) == 0); // OSF bit clerared = oscillator enabled
}

void RTCDS3231::enable32kHz(void) {
  byte stat = getRegister(DS3231_STATUS);
  setRegister(DS3231_STATUS, (stat & 0b11110111) | 0b00001000); // EN32kHz = 1
}

void RTCDS3231::disable32kHz(void) {
  byte stat = getRegister(DS3231_STATUS);
  setRegister(DS3231_STATUS, (stat & 0b11110111) | 0b00000000); //  EN32kHz = 0
}

void RTCDS3231::enable1Hz(void) {
  byte ctr = getRegister(DS3231_CONTROL);
  setRegister(DS3231_CONTROL, (ctr & 0b11100011) | 0b00000000); // RS1=0 RS=0 INTCN=0
}

void RTCDS3231::disable1Hz(void) {
  byte ctr = getRegister(DS3231_CONTROL);
  setRegister(DS3231_CONTROL, (ctr & 0b11100011) | 0b00000100); // RS1=0 RS=0 INTCN=1 disables 1 Hz
}

int RTCDS3231::getTemp(void) {
  byte temp = getRegister(DS3231_TEMPMSB);
  return (int8_t)temp;
}

// negative values make the clock faster by roughly 0.1 ppm/LSB,
// offset needs to be divided by ten since the common format is a step size of 0.01 ppm.
// Since there is only one mode, we ignore the mode parameter.
// After having changed the offset value, a conversion is gtriggered so that
// changes are immdiately visible. 
void RTCDS3231::setOffset(int offset, __attribute__ ((unused)) byte mode) {
  if (offset < 0) offset = (offset - 5)/10;
  else offset = (offset + 5)/10;
  if (offset < -128) offset = -128;
  else if (offset > 127) offset = 127;
  setRegister(DS3231_OFFSET, offset&0xFF);
  while (getRegister(DS3231_STATUS) & 0b100); // wait fcor non-busy period
  setRegister(DS3231_CONTROL,getRegister(DS3231_CONTROL)|0b100000);
}