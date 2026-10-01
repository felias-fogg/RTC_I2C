#include <RTC_RS5C372.h>

bool RS5C372::init(__attribute__((unused)) BatteryMode mode) {
  bool success = true;
  success &=
    setRegister(RS5C372_CONTROL1, 0b00100000); // INT(1Hz) and 32K to INTRB output, Alarm_A and Alarm_B to INTRA
  success &= setRegister(RS5C372_CONTROL2, 0b00101000); // 24h format and 32K output disabled
  success &= setRegister(RS5C372_OFFSET, 0);            // 32.768 crystal and no trimming
  return success;
}

bool RS5C372::isValid() {
  byte control;
  return readRegister(RS5C372_CONTROL2, control) &&
         ((control & 0b00010000) == 0); // XSTP=0, otherwise clock is/was halted
}

bool RS5C372::enableAlarm() {
  byte ctr;
  if (!readRegister(RS5C372_CONTROL1, ctr)) return false;
  return setRegister(RS5C372_CONTROL1, (ctr | 0b10000000)); // set AALE bit
}

bool RS5C372::disableAlarm() {
  byte ctr;
  if (!readRegister(RS5C372_CONTROL1, ctr)) return false;
  return setRegister(RS5C372_CONTROL1, (ctr & ~0b10000000)); // reset AALE bit
}

bool RS5C372::setAlarm(byte minute, byte hour) {
  bool success = true;
  success &= setRegister(RS5C372_ALARMMIN, bin2bcd(minute)); // set minute alarm
  success &= setRegister(RS5C372_ALARMHR, bin2bcd(hour));    // set hour alarm
  success &= setRegister(RS5C372_ALARMWDAYS, 0x7F);          // set day alarm to always
  return success;
}

bool RS5C372::senseAlarm() {
  byte control;
  return readRegister(RS5C372_CONTROL2, control) && ((control & 0b10) != 0);
}

bool RS5C372::clearAlarm() {
  byte ctr;
  if (!readRegister(RS5C372_CONTROL2, ctr)) return false;
  return setRegister(RS5C372_CONTROL2, (ctr & 0b11111101));
}


bool RS5C372::enable32kHz() {
  byte clkout;
  if (!readRegister(RS5C372_CONTROL2, clkout)) return false;
  return setRegister(RS5C372_CONTROL2, (clkout & ~0b00001000)); // set CLEN to 0
}

bool RS5C372::disable32kHz() {
  byte clkout;
  if (!readRegister(RS5C372_CONTROL2, clkout)) return false;
  return setRegister(RS5C372_CONTROL2, (clkout | 0b00001000)); // set CLEN to 1
}

bool RS5C372::enable1Hz() {
  byte clkout;
  if (!readRegister(RS5C372_CONTROL1, clkout)) return false;
  return setRegister(RS5C372_CONTROL1, (clkout & 0b11111000) | 0b00000011);
}

bool RS5C372::disable1Hz() {
  byte clkout;
  if (!readRegister(RS5C372_CONTROL1, clkout)) return false;
  return setRegister(RS5C372_CONTROL1, (clkout & 0b11111000));
}

bool RS5C372::setOffset(int offset, OffsetMode mode) {
  bool success = true;
  if (mode != OffsetMode::RAW_OFFSET) {
    // add and then divide to round the offset instead of truncating it
    offset = (offset + (offset > 0 ? +152 : -152)) / 305;
    // force offset into range
    if (offset < -64)
      offset = -64;
    else if (offset > 63)
      offset = 63;
  }
  // write the offset
  success &= setRegister(RS5C372_OFFSET, (offset & 0x7F)); // Serial.println(offset);
  // Serial.println(((offset&0x7F)|(mode<<7)));
  return success;
}

unsigned int RS5C372::getOffset() {
  return getRegister(RS5C372_OFFSET);
}

String RS5C372::getManufacturer() {
  return F("Ricoh");
}

String RS5C372::getModel() {
  return F("RS5C372");
}

// cSpell:ignore XSTP AALE
