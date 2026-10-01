#include <RTC_PCF8563.h>


bool PCF8563::init(__attribute__((unused)) BatteryMode mode) {
  bool success = true;
  success &= setRegister(PCF8563_CONTROL, 0);
  success &= setRegister(PCF8563_CONTROL + 1, 0);
  success &= setRegister(PCF8563_CLKOUT, 0);
  return success;
}

bool PCF8563::isValid() {
  byte control;
  byte seconds;
  bool success = true;
  success &= readRegister(PCF8563_CONTROL, control) && ((control & 0b00100000) == 0);
  //^ STOP not asserted
  success &= readRegister(PCF8563_CLOCKREG, seconds) && ((seconds & 0b10000000) == 0);
  //^ VL (low voltage) bit not asserted
  return success;
}

bool PCF8563::enableAlarm() {
  byte ctr;
  if (!readRegister(PCF8563_CONTROL + 1, ctr)) return false;
  return setRegister(PCF8563_CONTROL + 1, (ctr & 0b11111101) | 0b00000010);
}

bool PCF8563::disableAlarm() {
  byte ctr;
  if (!readRegister(PCF8563_CONTROL + 1, ctr)) return false;
  return setRegister(PCF8563_CONTROL + 1, (ctr & 0b11111101) | 0b00000000);
}


bool PCF8563::enable32kHz() {
  return setRegister(PCF8563_CLKOUT, 0b10000000);
}

bool PCF8563::disable32kHz() {
  return setRegister(PCF8563_CLKOUT, 0);
}

bool PCF8563::enable1Hz() {
  return setRegister(PCF8563_CLKOUT, 0b10000011);
}

bool PCF8563::disable1Hz() {
  return disable32kHz();
}

String PCF8563::getManufacturer() {
  return F("NXP");
}

String PCF8563::getModel() {
  return F("PCF8563");
}
