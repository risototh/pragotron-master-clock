#ifndef _CLI_COMMANDS_H_
  #define _CLI_COMMANDS_H_

  #include "cli.h"

  void cli_ssid_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_ssid_hlp();
  void cli_pass_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_pass_hlp();
  void cli_ntp_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_ntp_hlp();
  void cli_clock_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_clock_hlp();
  void cli_tz_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_tz_hlp();
  void cli_store_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_store_hlp();
  void cli_restart_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_restart_hlp();
  void cli_time_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_time_hlp();
  void cli_date_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_date_hlp();
  void cli_config_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_config_hlp();
  void cli_status_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_status_hlp();
  void cli_debug_cmd(char argv[CLI_MAX_NUM_ARGS][CLI_ARG_BUF_SIZE]);
  void cli_debug_hlp();

  /** console commands:
   *  HELP            - show help
   *  CLOCK <hhmm>    - set time, that is actually shown on the mechanical display
   *  TZ <offset>     - set timezone offset from UTC in minutes 
   *  TIME <hhmm>     - set RTC time
   *  DATE <yyyymmdd> - set RTC date
   *  SSID <ssid>     - set WiFi STA connection SSID
   *  PASS <password> - set WiFi STA connection password
   *  NTP <fqdn>      - set NTP server FQDN
   *  DEBUG (on|off)  - set debug on/off
   *  STORE           - store settings to EEPROM
   *  CONFIG          - show actual settings
   *  STATUS          - show status
   *  RESTART         - system restart
   */
  
  cliCommandItem cli_commands[] = {
    {"SSID",      &cli_ssid_cmd,      &cli_ssid_hlp},
    {"PASS",      &cli_pass_cmd,      &cli_pass_hlp},
    {"NTP",       &cli_ntp_cmd,       &cli_ntp_hlp},
    {"SYNC",      &cli_sync_cmd,      &cli_sync_hlp}
    {"CLOCK",     &cli_clock_cmd,     &cli_clock_hlp},
    {"TZ",        &cli_tz_cmd,        &cli_tz_hlp},
    {"TIME",      &cli_time_cmd,      &cli_time_hlp},
    {"DATE",      &cli_date_cmd,      &cli_date_hlp},
    {"STORE",     &cli_store_cmd,     &cli_store_hlp},
    {"CONFIG",    &cli_config_cmd,    &cli_config_hlp},
    {"STATUS",    &cli_status_cmd,    &cli_status_hlp},
    {"DEBUG",     &cli_debug_cmd,     &cli_debug_hlp},
    {"RESTART",   &cli_restart_cmd,   &cli_restart_hlp}

  };
  
#endif
