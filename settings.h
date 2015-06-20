#ifndef settings_h
#define settings_h

#include <Arduino.h>
#include <inttypes.h>
#include <EEPROM.h>

#define CONFIG_VERSION "002"
#define CONFIG_START 0
 
struct SettingsStruct {
  // This is for detection if they are your settings
  char version[4];
  // The variables of your settings
  bool APmode;
  char ssid[30];
  char password[30];
  char name[30];
  bool useWDT;
  bool runLED;
  bool SerDebug;
  bool LcdDebug;
  bool deepSleep;
  uint32_t deepSleepTime;
  bool allowPUT;
  bool autoGET;
  uint16_t getInterval;
  char host[30];
  char url[30];
};

void initConfig(void);
void loadConfig(void);
void saveConfig(void);

#endif
