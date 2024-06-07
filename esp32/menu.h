#pragma once

#define MENU_RUN 0
#define MENU_CALIBRATE 1
#define MENU_INVOKE_EEPROM 2

namespace menu {
  extern bool DisplayInit();
  extern void showDifference(int16_t value, bool clear);
  extern int menu();
  extern void showWaiting(const char* msg);
  extern void showRotation();
}