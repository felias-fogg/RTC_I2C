#include <RTC_DS3231.h>

bool DS3231::init(__attribute__((unused)) BatteryMode mode) {
  bool success = true;
  success &= setRegister(DS3231_CONTROL,
                         0b00000100); // typical value after power-on, except for bit 2 (disables SQW), starts clock
  success &= setRegister(DS3231_STATUS, 0b00000000); // clear OSF flag, clear alarm flags, disable 32 kHz output
  return success;
}

bool DS3231::isValid() {
  byte status;
  return readRegister(DS3231_STATUS, status) && ((status & 0x80) == 0); // OSF bit cleared = oscillator enabled
}

bool DS3231::enable32kHz() {
  byte stat;
  if (!readRegister(DS3231_STATUS, stat)) return false;
  return setRegister(DS3231_STATUS, (stat & 0b11110111) | 0b00001000); // EN32kHz = 1
}

bool DS3231::disable32kHz() {
  byte stat;
  if (!readRegister(DS3231_STATUS, stat)) return false;
  return setRegister(DS3231_STATUS, (stat & 0b11110111) | 0b00000000); //  EN32kHz = 0
}

bool DS3231::enable1Hz() {
  byte ctr;
  if (!readRegister(DS3231_CONTROL, ctr)) return false;
  return setRegister(DS3231_CONTROL, (ctr & 0b11100011) | 0b00000000); // RS1=0 RS=0 INTCN=0
}

bool DS3231::disable1Hz() {
  byte ctr;
  if (!readRegister(DS3231_CONTROL, ctr)) return false;
  return setRegister(DS3231_CONTROL, (ctr & 0b11100011) | 0b00000100); // RS1=0 RS=0 INTCN=1 disables 1 Hz
}

int DS3231::getTemp() {
  byte temp;
  if (!readRegister(DS3231_TEMPMSB, temp)) return -128;
  return (int8_t)temp;
}

bool DS3231::setOffset(int offset, OffsetMode mode) {
  bool success = true;
  byte timeout = 0;
  if (mode != OffsetMode::RAW_OFFSET) {
    //  If the user didn't specify that this is a raw offset, assume they gave an offset in the more common format of
    //  0.01 ppm steps.
    if (offset < 0)
      offset = (offset - 5) / 10;
    else
      offset = (offset + 5) / 10;
    // force the offset to be in the supported range
    if (offset < -128)
      offset = -128;
    else if (offset > 127)
      offset = 127;
  }
  success &= setRegister(DS3231_OFFSET, offset & 0xFF);
  while (++timeout && (getRegister(DS3231_STATUS) & 0b100)); // wait for non-busy period
  if (!timeout) return false;
  byte control;
  if (!readRegister(DS3231_CONTROL, control)) return false;
  success &= setRegister(DS3231_CONTROL, control | 0b100000);
  return success;
}

unsigned int DS3231::getOffset() {
  return getRegister(DS3231_OFFSET);
}

String DS3231::getManufacturer() {
  return F("Analog Devices");
}

String DS3231::getModel() {
  return F("DS3231");
}

// cSpell:ignore TEMPMSB
