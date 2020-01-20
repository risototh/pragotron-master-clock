#include "storage.h"
#include <EEPROM.h>

struct EEPROMSettings loadSettings() {
  EEPROMSettings settings; 

  EEPROM.begin(sizeof(EEPROMSettings));
  EEPROM.get(CONFIG_START, settings);
  
  if (strcmp(settings.version, CONFIG_VERSION) == 0) {
    if (settings.debug) {
      CONSOLE.println("  > loaded settings data: ");
      printSettingRaw(settings);
      CONSOLE.println();
    }
    EEPROM.end();
    return settings;
  }

  // settings not found, so try to write the defaults
  EEPROMSettings newSettings;
  CONSOLE.println("  > not found");
  CONSOLE.println("  > writing default data");
  storeSettings(newSettings); // defaults are defined in the structure definition
  return newSettings;
}

void storeSettings(struct EEPROMSettings settingsIn) {
  noInterrupts();
  struct EEPROMSettings settings = settingsIn;
  interrupts();

  EEPROM.begin(sizeof(EEPROMSettings));
  EEPROM.put(CONFIG_START, settings);
  EEPROM.end();

  if (settings.debug) {
    CONSOLE.println("  > stored settings data: ");
    printSettingRaw(settings);
    CONSOLE.println();
  }
}

void printSettingRaw(struct EEPROMSettings settings) {
  for (unsigned int t = 0; t < sizeof(EEPROMSettings); t++) {
    if ((char)(*((char*)&settings + t)) < 10) CONSOLE.print("0");
    CONSOLE.print((char)(*((char*)&settings + t)), HEX);
    if (t % 16 == 15) {
      CONSOLE.println();
    }
    else {
      CONSOLE.print(" ");
    }
  }
}

struct NVRAMState loadState() {
  struct NVRAMState state;

  return state;
}

void storeState(struct NVRAMState state) {
  
}
  
