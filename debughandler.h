#ifndef debughandler_h
#define debughandler_h
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define YMAX 310

void debug_init(void);
void debugPrint(const char*, unsigned int color=ILI9341_WHITE);
void debugPrintln(const char*, unsigned int color=ILI9341_WHITE);

#endif

