#include "cli.h"
#include "storage.h"
#include <ESP8266WiFi.h>
#include <RTClib.h>

#define PRINTLN_VAL(x,y) CLI_CONSOLE.print((x)); CLI_CONSOLE.println((y))

extern EEPROMSettings systemSettings;
extern NVRAMState systemState;
extern RTC_DS3231 rtc;

void cli_ssid_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {
  strncpy(systemSettings.wifi_sta_ssid, argv[1], sizeof(systemSettings.wifi_sta_ssid));
  CLI_CONSOLE.print("SSID set to \"");
  CLI_CONSOLE.print(systemSettings.wifi_sta_ssid);
  CLI_CONSOLE.println("\"");
}

void cli_ssid_hlp() {
  CLI_CONSOLE.println("SSID <ssid>\nSet WiFI SSID to connect to.");
}

void cli_pass_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {
  strncpy(systemSettings.wifi_sta_pass, argv[1], sizeof(systemSettings.wifi_sta_pass));
  CLI_CONSOLE.print("Password set to \"");
  CLI_CONSOLE.print(systemSettings.wifi_sta_pass);
  CLI_CONSOLE.println("\"");
}

void cli_pass_hlp() {
  CLI_CONSOLE.println("PASS <password>\nSet WiFI password.");
}

void cli_clock_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {
  CLI_CONSOLE.println("Clock... has to be implemented :D");
  CLI_CONSOLE.println(argv[1]);
}

void cli_clock_hlp() {
  CLI_CONSOLE.println("CLOCK <hhmm>\nSet the current time, that is shown on the mechanical display (clock).");
}

void cli_store_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {
  storeSettings(systemSettings);
  CLI_CONSOLE.println("Settings stored to EEPROM.");
}

void cli_store_hlp() {
  CLI_CONSOLE.println("Store settings to EEPROM.");
}

void cli_restart_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {
  CLI_CONSOLE.println("Restarting...");
  ESP.restart();
}

void cli_restart_hlp() {
  CLI_CONSOLE.println("Will induce the system restart.");
}

void cli_ntp_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {
  strncpy(systemSettings.ntp_server, argv[1], sizeof(systemSettings.ntp_server));
  CLI_CONSOLE.print("NTP server set to \"");
  CLI_CONSOLE.print(systemSettings.ntp_server);
  CLI_CONSOLE.println("\"");
}

void cli_ntp_hlp() {
  CLI_CONSOLE.println("NTP <fqdn>\nSet NTP server FQDN.");
}

void cli_tz_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {

}

void cli_tz_hlp() {
  CLI_CONSOLE.println("TZ <offset>\nSet timezone offset from UTC in minutes.");
}

void cli_time_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {

}

void cli_time_hlp() {
  CLI_CONSOLE.println("TIME <hhmm>\nSet RTC time.");
}

void cli_date_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {

}

void cli_date_hlp() {
  CLI_CONSOLE.println("DATE <yyyymmdd>\nSet RTC date.");
}

void cli_config_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {
  PRINTLN_VAL("WiFi SSID:     ", systemSettings.wifi_sta_ssid);
  PRINTLN_VAL("WiFi password: ", systemSettings.wifi_sta_pass);
  PRINTLN_VAL("NTP server:    ", systemSettings.ntp_server);
  PRINTLN_VAL("TZ offset:     ", systemSettings.tz_offset);
  PRINTLN_VAL("Debug:         ", systemSettings.debug);
}

void cli_config_hlp() {
  CLI_CONSOLE.println("Show actual settings.");
}

void cli_status_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {
  char t[32];
  DateTime dt = rtc.now();
  sprintf(t, "%04d-%02d-%02d %02d:%02d:%02d",  dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());  
  PRINTLN_VAL("RTC time:            ", t);

  dt = DateTime(systemState.lastSync);
  sprintf(t, "%04d-%02d-%02d %02d:%02d:%02d",  dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());  
  PRINTLN_VAL("Last NTP sync time:  ", t);

  PRINTLN_VAL("Last drive polarity: ", systemState.drivePolarity);

  dt = DateTime(0, 0, 0, systemState.displayTime / 60, systemState.displayTime % 60, 0);
  sprintf(t, "%02d:%02d",  dt.hour(), dt.minute());  
  PRINTLN_VAL("Display time:        ", t);

  CLI_CONSOLE.println("WiFi status:");
  WiFi.printDiag(CONSOLE);
}

void cli_status_hlp() {
  CLI_CONSOLE.println("Show system status.");
}

void cli_debug_hlp() {
  CLI_CONSOLE.println("DEBUG <on/off>\nSet debug on/off.");
}

void cli_debug_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]) {
  if (strcmp(argv[1], "on") == 0) {
    systemSettings.debug = 1;
    CLI_CONSOLE.println("Debug set to on.");
    return;
  }
  if (strcmp(argv[1], "off") == 0) {
    systemSettings.debug = 0;
    CLI_CONSOLE.println("Debug set to off.");
    return;
  }

  CLI_CONSOLE.println("Unknown value.");
  cli_debug_hlp();
}
