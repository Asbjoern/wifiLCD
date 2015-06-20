#include "debughandler.h"
#include "settings.h"
#include <Arduino.h>
#include <String>
#include <TFTv2.h>

unsigned int lcdx;
unsigned int lcdy;

extern struct SettingsStruct settings;

void debug_init()
{
  lcdx =0;
  lcdy =0;
}
 
void debugPrint(const char* str, unsigned int color)
{
  if(settings.SerDebug == true)
    Serial.print(str);
  if(settings.LcdDebug){
    Tft.fillScreen(lcdx,lcdx+strlen(str)*LCDFONTX,lcdy,lcdy+LCDFONTY,BLACK);
    Tft.drawString((char*)str,lcdx,lcdy,LCDFONTSIZE,color);
    lcdx += strlen(str)*LCDFONTX;
    if(lcdx > MAXX){
      lcdy +=LCDFONTY;
      lcdx = 0;
      if(lcdy > MAXY)
        lcdy = 0;
    }
  }
}

void debugPrintln(const char* str, unsigned int color)
{
  if(settings.SerDebug)
    Serial.println(str);
  if(settings.LcdDebug){
    Tft.fillScreen(lcdx,240,lcdy,lcdy+LCDFONTY,BLACK);
    Tft.drawString((char*)str,lcdx,lcdy,LCDFONTSIZE,color);
    lcdx = 0;
    lcdy +=LCDFONTY;
    if(lcdy > MAXY)
      lcdy = 0;
  }
}
