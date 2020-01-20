#ifndef _STORAGE_H_
  #define _STORAGE_H_

  #include "config.h"
  #include <Arduino.h>

  struct EEPROMSettings {
    char version[5] = CONFIG_VERSION;
    char wifi_sta_ssid[33] = DEFAULT_SSID;
    char wifi_sta_pass[65] = DEFAULT_PASS;
    char ntp_server[256] = DEFAULT_NTP_SERVER;
    int tz_offset = DEFAULT_TZ_OFFSET; // timezone offset from UTC in minutes
    byte debug = 1;
  };

  struct NVRAMState {
    char version[5] = CONFIG_VERSION;
    unsigned int displayTime = 0;
    byte drivePolarity = 0;
    unsigned long lastSync = 0;
  };

  struct EEPROMSettings loadSettings();
  void storeSettings(struct EEPROMSettings settingsIn);
  void printSettingRaw(struct EEPROMSettings settings);

  struct NVRAMState loadState();
  void storeState(struct NVRAMState state);

#endif
