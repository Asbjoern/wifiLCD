#include "debughandler.h"
#include "settings.h"
#include <Arduino.h>
//#include <String>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

extern struct SettingsStruct settings;
extern Adafruit_ILI9341 tft;

WiFiServer TelnetServer(settings.TelnetPort);
WiFiClient Telnet;

void debug_init()
{
  if(settings.TelnetDebug == true)
  {
    TelnetServer.begin();
    TelnetServer.setNoDelay(true);
  }
  
}

void debug_handleTelnet()
{
  if(settings.TelnetDebug == true)
  {
    if (TelnetServer.hasClient()){
      if (!Telnet || !Telnet.connected()){
        if(Telnet) Telnet.stop();
        Telnet = TelnetServer.available();
      } else {
        WiFiClient toKill = TelnetServer.available();
        toKill.stop();
      }
    }
    if (Telnet && Telnet.connected() && Telnet.available()){
      while(Telnet.available())
        Telnet.read();
    }
  }
}
 
void debugPrint(const char* str, unsigned int color)
{
  if(settings.SerDebug == true)
    Serial.print(str);
  if(settings.LcdDebug){
    if(tft.getCursorY() > YMAX)
      tft.setCursor(0,0);
    tft.setTextSize(2);
    tft.setTextColor(color,ILI9341_BLACK);
    tft.print(str);
  }
  if(settings.TelnetDebug)
  {
    if (Telnet && Telnet.connected()){
      Telnet.print(str);
      yield();
    }
  }
}

void debugPrintln(const char* str, unsigned int color)
{
  if(settings.SerDebug)
    Serial.println(str);
  if(settings.LcdDebug){
    if(tft.getCursorY() > YMAX)
      tft.setCursor(0,0);
    tft.setTextSize(2);
    tft.setTextColor(color,ILI9341_BLACK);
    tft.println(str);
  }
  if(settings.TelnetDebug)
  {
    if (Telnet && Telnet.connected()){
      Telnet.println(str);
      yield();
    }
  }
}
