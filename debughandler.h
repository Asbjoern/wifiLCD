#ifndef debughandler_h
#define debughandler_h

#include <TFTv2.h>

#define LCDFONTSIZE 2
#define LCDFONTX 6*LCDFONTSIZE
#define LCDFONTY 10*LCDFONTSIZE
#define MAXX 240-LCDFONTX
#define MAXY 320-LCDFONTY

void debug_init(void);
void debugPrint(const char*, unsigned int color=WHITE);
void debugPrintln(const char*, unsigned int color=WHITE);

#endif

