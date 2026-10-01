#include <RTC_PCF8523.h>

// if Vbat disabled, connect to Vcc
bool PCF8523::init(BatteryMode mode) {
  bool success = true;
  success &= setRegister(PCF8523_CONTROL, 0b01011000); // initiate software reset with the documented 0x58 command
  success &= setRegister(PCF8523_CONTROL + 1, 0);      // disable watchdog and countdown timers
  byte bsm_reg = 0;
  switch (mode) {
  case BatteryMode::SWITCHING_DISABLED: {
    bsm_reg = 0b01100000;
    break;
  }
  case BatteryMode::LEVEL_SWITCHING: {
    bsm_reg = 0b00000000;
    break;
  }
  case BatteryMode::DIRECT_SWITCHING: {
    bsm_reg = 0b00100000;
    break;
  }
  }
  success &= setRegister(PCF8523_CONTROL + 2, bsm_reg); // switch mode and battery low detection
  success &= setRegister(PCF8523_CLKOUT, 0b00111000);   // disable clock output
  return success;
}

bool PCF8523::isValid() {
  byte control1;
  byte control3;
  byte seconds;
  bool success = true;
  success &= readRegister(PCF8523_CONTROL + 2, control3) && ((control3 & 0b00000100) == 0); // battery OK
  success &= readRegister(PCF8523_CONTROL, control1) && ((control1 & 0b00100000) == 0);     // oscillator running
  success &= readRegister(PCF8523_CLOCKREG, seconds) && ((seconds & 0b10000000) == 0);      // OS flag cleared
  return success;
}

bool PCF8523::enableAlarm() {
  byte ctr;
  if (!readRegister(PCF8523_CONTROL, ctr)) return false;
  return setRegister(PCF8523_CONTROL, (ctr & 0b11111101) | 0b00000010);
}

bool PCF8523::disableAlarm() {
  byte ctr;
  if (!readRegister(PCF8523_CONTROL, ctr)) return false;
  return setRegister(PCF8523_CONTROL, (ctr & 0b11111101) | 0b00000000);
}


bool PCF8523::enable32kHz() {
  byte clkout;
  if (!readRegister(PCF8523_CLKOUT, clkout)) return false;
  return setRegister(PCF8523_CLKOUT, (clkout & 0b11000111) | 0b00000000);
}

bool PCF8523::disable32kHz() {
  byte clkout;
  if (!readRegister(PCF8523_CLKOUT, clkout)) return false;
  return setRegister(PCF8523_CLKOUT, (clkout & 0b11000111) | 0b00111000);
}

bool PCF8523::enable1Hz() {
  byte clkout;
  if (!readRegister(PCF8523_CLKOUT, clkout)) return false;
  return setRegister(PCF8523_CLKOUT, (clkout & 0b11000111) | 0b00110000);
}

bool PCF8523::disable1Hz() {
  return disable32kHz();
}

bool PCF8523::setOffset(int offset, OffsetMode mode) {
  bool success = true;
  if (mode == OffsetMode::RAW_OFFSET)
    // put the raw value into the register
    success &= setRegister(PCF8523_OFFSET, (offset & 0xFF));
  else {
    if (mode == OffsetMode::COARSE_OFFSET)
      // add and then divide to round the offset instead of truncating it
      offset = (offset + (offset > 0 ? +217 : -217)) / 434;
    else
      // add and then divide to round the offset instead of truncating it
      offset = (offset + (offset > 0 ? +203 : -203)) / 406;
    // force offset into range
    if (offset < -64)
      offset = -64;
    else if (offset > 63)
      offset = 63;
    // set the offset register with the calculated value and mode
    success &=
      setRegister(PCF8523_OFFSET, ((offset & 0x7F) | (static_cast<byte>(mode) << 7))); // Serial.println(offset);
    // Serial.println(((offset&0x7F)|(mode<<7)));
  }
  return success;
}

unsigned int PCF8523::getOffset() {
  return getRegister(PCF8523_OFFSET);
}

String PCF8523::getManufacturer() {
  return F("NXP");
}

String PCF8523::getModel() {
  return F("PCF8523");
}

// cSpell:ignore  VBat
