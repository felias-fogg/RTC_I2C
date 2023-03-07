#include <RTC_SD2405.h>

void SD2405::init(__attribute__ ((unused)) byte mode) {
  setRegister(SD2405_CONTROL+1, 0x80); // unlock RTC
  setRegister(SD2405_CONTROL, 0x84); // unlock RTC
  setRegister(SD2405_CONTROL+2, 0x00);
}

bool SD2405::isValid(void) {
  return ((getRegister(SD2405_CONTROL) & 0b1) == 0); // power on bit (not sure, but believe that = 0 means valid)
}

void SD2405::setAlarm(byte minute, byte hour) {
  setRegister(SD2405_ALARM+1, bin2bcd(minute)); // set second alarm
  setRegister(SD2405_ALARM+2, bin2bcd(hour)); // set hour alarm
  setRegister(SD2405_ALARM+7, 0b110); // hours and minute need to match
}

void SD2405::setAlarm(byte minute) {
  setRegister(SD2405_ALARM+1, bin2bcd(minute)); // set second alarm
  setRegister(SD2405_ALARM+7, 0b010); // minute needs to match
}


void SD2405::enableAlarm(void) { 
  byte ctr = getRegister(SD2405_CONTROL+1);
  setRegister(SD2405_CONTROL+1, (ctr & 0b11001101) | 0b00010010); // set the INTAE bit and INTS1/INTS=01
  
}

void SD2405::disableAlarm(void) {
  byte ctr = getRegister(SD2405_CONTROL+1);
  setRegister(SD2405_CONTROL+1, (ctr & 0b11111101));  // clear INTAE bit
}

bool SD2405::senseAlarm(void) {
  return ((getRegister(SD2405_CONTROL) & 0b00100000) != 0);
}

void SD2405::clearAlarm(void) {
  byte ctr = getRegister(SD2405_ALARM+3);
  setRegister(SD2405_ALARM+3, (ctr & 0b11011111)); 
}


void SD2405::enable32kHz(void) {
  setRegister(SD2405_CONTROL+1, (getRegister(SD2405_CONTROL+1) & 0b11001110) | 0b00100001); // enable SQW output
  setRegister(SD2405_CONTROL+2, (getRegister(SD2405_CONTROL+2) & 0b11110000) | 0b0001); // 32kHz
}

void SD2405::disable32kHz(void) {
  setRegister(SD2405_CONTROL+1, getRegister(SD2405_CONTROL+1) & ~0b00110001); // disable SQW and alarm INT
}

void SD2405::enable1Hz(void) {
  setRegister(SD2405_CONTROL+1, (getRegister(SD2405_CONTROL+1) & 0b11001110) | 0b00100001); // enable SQW output
  setRegister(SD2405_CONTROL+2, (getRegister(SD2405_CONTROL+2) & 0b11110000) | 0b1010); // 1 Hz
}



void SD2405::disable1Hz(void) {
  disable32kHz();
}



// negative values make the clock faster by roughly 3.051 ppm/LSB.
// The range of the internal parameter goes from -64 to +63, 
void SD2405::setOffset(int offset, byte mode) {
  if (mode != 2) {
    offset = (offset + (offset > 0 ? +152 : -152))/305;
    if (offset < -62) offset = -62;
    else if (offset > 63) offset = 63;
  }
  //Serial.println(offset);
  setRegister(SD2405_OFFSET, (offset&0x7F));
}

  
