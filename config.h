#ifndef _CONFIG_H_
  #define _CONFIG_H_

  #define CONSOLE Serial
  #define PIN_LED_STATUS     D0
  #define PIN_SCL            D1
  #define PIN_SDA            D2
  #define PIN_DST_SWITCH     D3
  #define PIN_PULSE_1        D5
  #define PIN_PULSE_2        D6
  #define CONNECTION_TIMEOUT 15000 // timeout to connect to WiFi in ms
  #define NTP_TIMEOUT        1000 // timeout to receive NTP response in ms
  #define NTP_PACKET_SIZE    48 // NTP time stamp is in the first 48 bytes of the message
  #define NTP_LOCAL_PORT     2390
  #define CONFIG_VERSION     "PT03"
  #define CONFIG_START       0

  #define DEFAULT_SSID       "fifina"
  #define DEFAULT_PASS       "seatec-astronomy"
  #define DEFAULT_NTP_SERVER "tik.cesnet.cz"
  #define DEFAULT_TZ_OFFSET  60
  
#endif
