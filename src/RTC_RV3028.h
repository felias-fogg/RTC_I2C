/* This is the class for RV3028 that can be used as part of the RTC_I2C library */

#ifndef _RTC_RV3028_H_
#define _RTC_RV3028_H_

#include <RTC_I2C.h>
#include <util/delay.h>


#define RV3028_ADDRESS 0x52 // I2C address for RV3028
#define RV3028_CLOCKREG 0x00 // Clock register (that is where the seconds start)
#define RV3028_ALARM   0x07 // Alarm minutes register
#define RV3028_STATUS 0x0E // Status register
#define RV3028_CONTROL 0x0F // Control register
#define RV3028_CLKOUT 0x35 // Clockout enable & and FD
#define RV3028_BSM 0x37 // Clockout enable & BSM register
#define RV3028_OFFSET  0x36 // Offset register
#define RV3028_EECMD   0x27 // EEPROM command
#define RV3028_EEDATA  0x26 // value for  EEPROM data transfer 
#define RV3028_EEADDR  0x25 // address for EEPROM data transfer

#define RV3028_WDAYBASE 0    // wday range from 0 to 6, 
#define RV3028_WDAYFIRST true    // wday comes after day of month in clock reg
#define RV3028_BIT7 0
#define RV3028_CAP  (RTC_CAP_32KHZ|RTC_CAP_1HZ|RTC_CAP_ALARM|RTC_CAP_OFFSET)


class RV3028: public RTC {
 public:
  RV3028(void)  {
    _i2caddr = RV3028_ADDRESS;
    _clockreg = RV3028_CLOCKREG;
    _wdaybase = RV3028_WDAYBASE;
    _wdayfirst = RV3028_WDAYFIRST;
    _capabilities = RV3028_CAP;
    _bit7set =  RV3028_BIT7;
  };
  void init(byte mode=1);
  bool isValid(void);
  virtual void setAlarm(byte minute, byte hour);
  virtual bool senseAlarm(void);
  virtual void clearAlarm(void);
  virtual void enableAlarm(void);
  virtual void disableAlarm(void);
  void enable32kHz(void);
  void disable32kHz(void);
  void enable1Hz(void);
  void disable1Hz(void);
  void setOffset(int offset, byte mode=1);

 protected:
  void updateEEPROMByte(byte reg);
  
};
#endif
