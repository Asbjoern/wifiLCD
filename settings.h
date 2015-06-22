#ifndef settings_h
#define settings_h

#include <Arduino.h>
#include <inttypes.h>
#include <EEPROM.h>

#define CONFIG_VERSION "003"
#define CONFIG_START 0
 
struct SettingsStruct {
  // This is for detection if they are your settings
  char version[4];
  // The variables of your settings
  bool APmode;
  char ssid[32];
  char password[32];
  char name[32];
  bool useWDT;
  bool runLED;
  bool SerDebug;
  bool LcdDebug;
  bool deepSleep;
  uint32_t deepSleepTime;
  uint16_t backlight;
  bool allowPUT;
  bool autoGET;
  uint16_t getInterval;
  char host[32];
  char url[32];
};

void initConfig(void);
void loadConfig(void);
void saveConfig(void);

#endif
