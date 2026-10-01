#include <RTC_SD2405.h>

bool SD2405::init(__attribute__((unused)) BatteryMode mode) {
  bool success = true;
  success &= setRegister(SD2405_CONTROL + 1, 0x80); // unlock RTC
  success &= setRegister(SD2405_CONTROL, 0x84);     // unlock RTC
  success &= setRegister(SD2405_CONTROL + 2, 0x00);
  return success;
}

bool SD2405::isValid() {
  byte control;
  return readRegister(SD2405_CONTROL, control) &&
         ((control & 0b1) == 0); // RTCF=0 means no oscillator failure has been detected
}

bool SD2405::setAlarm(byte minute, byte hour) {
  bool success = true;
  success &= setRegister(SD2405_ALARM + 1, bin2bcd(minute)); // set minute alarm
  success &= setRegister(SD2405_ALARM + 2, bin2bcd(hour));   // set hour alarm
  success &= setRegister(SD2405_ALARM + 7, 0b110);           // hours and minute need to match
  return success;
}

bool SD2405::setAlarm(byte minute) {
  bool success = true;
  success &= setRegister(SD2405_ALARM + 1, bin2bcd(minute)); // set minute alarm
  success &= setRegister(SD2405_ALARM + 7, 0b010);           // minute needs to match
  return success;
}


bool SD2405::enableAlarm() {
  byte ctr;
  if (!readRegister(SD2405_CONTROL + 1, ctr)) return false;
  return setRegister(SD2405_CONTROL + 1, (ctr & 0b11001101) | 0b00010010); // set the INTAE bit and INTS1/INTS=01
}

bool SD2405::disableAlarm() {
  byte ctr;
  if (!readRegister(SD2405_CONTROL + 1, ctr)) return false;
  return setRegister(SD2405_CONTROL + 1, (ctr & 0b11111101)); // clear INTAE bit
}

bool SD2405::senseAlarm() {
  byte control;
  return readRegister(SD2405_CONTROL, control) && ((control & 0b00100000) != 0);
}

bool SD2405::clearAlarm() {
  byte ctr;
  if (!readRegister(SD2405_CONTROL, ctr)) return false;
  return setRegister(SD2405_CONTROL, (ctr & 0b11011111)); // clear INTAF
}


bool SD2405::enable32kHz() {
  byte control2;
  byte control3;
  if (!readRegister(SD2405_CONTROL + 1, control2) || !readRegister(SD2405_CONTROL + 2, control3)) return false;
  bool success = setRegister(SD2405_CONTROL + 1, (control2 & 0b11001110) | 0b00100001); // enable SQW output
  success &= setRegister(SD2405_CONTROL + 2, (control3 & 0b11110000) | 0b0001);         // 32kHz
  return success;
}

bool SD2405::disable32kHz() {
  byte control;
  if (!readRegister(SD2405_CONTROL + 1, control)) return false;
  return setRegister(SD2405_CONTROL + 1, control & ~0b00110001); // disable frequency output and INT selection
}

bool SD2405::enable1Hz() {
  byte control2;
  byte control3;
  if (!readRegister(SD2405_CONTROL + 1, control2) || !readRegister(SD2405_CONTROL + 2, control3)) return false;
  bool success = setRegister(SD2405_CONTROL + 1, (control2 & 0b11001110) | 0b00100001); // enable SQW output
  success &= setRegister(SD2405_CONTROL + 2, (control3 & 0b11110000) | 0b1010);         // 1 Hz
  return success;
}


bool SD2405::disable1Hz() {
  return disable32kHz();
}


bool SD2405::setOffset(int offset, OffsetMode mode) {
  bool success = true;
  if (mode != OffsetMode::RAW_OFFSET) {
    // add and then divide to round the offset instead of truncating it
    offset = (offset + (offset > 0 ? +152 : -152)) / 305;
    // force the offset into range
    if (offset < -62)
      offset = -62;
    else if (offset > 63)
      offset = 63;
  }
  // set the offset register
  success &= setRegister(SD2405_OFFSET, (offset & 0x7F)); // Serial.println(offset);
  return success;
}

unsigned int SD2405::getOffset() {
  return (getRegister(SD2405_OFFSET) & 0x7F);
}

String SD2405::getManufacturer() {
  return F("DFRobot");
}

String SD2405::getModel() {
  return F("SD2405");
}

// cSpell:ignore INTAE
