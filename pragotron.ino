#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>
#include "config.h"

/** console commands:
 *  HELP
 *  CLOCK <hhmm>    - nastav momentalne zobrazeny udaj na "displeji"
 *  TZ <offset>     - nastav offset v minutach od UTC   
 *  SSID <ssid>     - nastav nove SSID na WiFi
 *  PASS <password> - nastav nove heslo na WiFi
 *  NTP <fqdn>      - NTP server FQDN
 *  DEBUG (on|off)  - zapni/vypni debug
 *  STORE           - uloz nastavenia
 *  CONFIG          - zobraz nastavenia
 *  STATUS          - zobraz stav
 *  RESTART         - restart
 */

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
bool debug = true;
bool output_polarity = 0;
Settings systemSettings;

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
RTC_DS3231 rtc;

void setup() {
  CONSOLE.begin(115200);
  CONSOLE.print("\n\n\n\n");
  CONSOLE.println("PRAGOTRON NTP Master Clock\nby risototh\n");
  CONSOLE.println("Init start");

  initSettings();
  initIO();
  initRTC();
  initWiFi();
  initNTP();  

  CONSOLE.print("Init end\n\n");
}

void loop() {
  ntpQuery();
  // wait ten seconds before asking for the time again
  delay(10000);
}

void initSettings() {
  CONSOLE.println("- Settings");
  systemSettings = loadSettings();
}

void initIO() {
  CONSOLE.println("- I/O");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn off led
  pinMode(PIN_LED_STATUS, OUTPUT);
  pinMode(PIN_DST_SWITCH, INPUT_PULLUP);
  pinMode(PIN_PULSE_1, OUTPUT);
  pinMode(PIN_PULSE_2, OUTPUT);
}

void initWiFi() {
  CONSOLE.println("- WiFi");
  wifiConnect(); // neskor presunut do nejakeho vlakna, lebo na beh hodin nie je nevyhnutne nutne pripojenie na wifi!
}

void initNTP() {
  CONSOLE.println("- NTP");
  if (WiFi.status() != WL_CONNECTED) {
    CONSOLE.println("  > WiFi connection not available");
    return;
  }
  udp.begin(NTP_LOCAL_PORT);
  CONSOLE.print("  > local port: "); CONSOLE.println(udp.localPort());
}

void initRTC() {
  CONSOLE.println("- RTC");
  Wire.begin();
  if (!rtc.begin()) {
    CONSOLE.println("  > DS3231 not found - RESTARTING!!!");
    ESP.restart();
  }
  rtc.writeSqwPinMode(Ds3231SqwPinMode::DS3231_OFF);
  //CONSOLE.println("  > setting time to compile time");
  //rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  
  CONSOLE.print("  > local time = ");
  printDateTime(rtc.now());
  CONSOLE.println();
  CONSOLE.print("  > TCXO temperature = ");
  CONSOLE.println(rtc.getTemperature());
}

void printDateTime(DateTime now) {
  char t[32];
  sprintf(t, "%04d-%02d-%02d %02d:%02d:%02d",  now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());  
  CONSOLE.print(t);
}

void ntpQuery() {
  digitalWrite(PIN_LED_STATUS, HIGH);

  IPAddress timeServerIP; // NTP server address - will be resolved from systemSettings.ntp_server
  WiFi.hostByName(systemSettings.ntp_server, timeServerIP); //get a random server from the pool
  
  ntpSend(timeServerIP); // send an NTP packet to a time server
  unsigned long unixTime = ntpReceive(); // wait to see if a reply is available

  // sync RTC
  if (unixTime > SECONDS_FROM_1970_TO_2000) { // valid time, year > 2000 :D
    DateTime dt = DateTime(unixTime);
    dt = dt + TimeSpan(systemSettings.tz_offset * 60);
    rtc.adjust(dt);
    if (systemSettings.debug) {
      CONSOLE.print("NTP: adjusting RTC time to ");
      printDateTime(dt);
      CONSOLE.println();
    }
  }
  
  digitalWrite(PIN_LED_STATUS, LOW);
  if (systemSettings.debug) CONSOLE.println();
}

unsigned long ntpReceive() {
  long rtt = 0, timeoutStart = millis();
  int cb = 0;
  while (!cb && rtt < NTP_TIMEOUT) {
    cb = udp.parsePacket();
    rtt = millis() - timeoutStart;
  }
  
  if (!cb) {
    if (systemSettings.debug) CONSOLE.println("NTP: timeout");
    return 0;
  } 
  
  if (systemSettings.debug) {
    CONSOLE.print("NTP: packet received, length = "); CONSOLE.print(cb);
    CONSOLE.print(" B, RTT = "); CONSOLE.print(rtt); CONSOLE.println(" ms");
  }
  
  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  // the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  if (systemSettings.debug) {
    CONSOLE.print("NTP: seconds since Jan 1 1900 = "); CONSOLE.println(secsSince1900);
  }

  // now convert NTP time into everyday time:
  
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;
  // print Unix time:
  if (systemSettings.debug) {
    CONSOLE.print("NTP: Unix timestamp = "); CONSOLE.println(epoch);
  }

  if (systemSettings.debug) {
    // print the hour, minute and second:
    CONSOLE.print("NTP: UTC time = ");       // UTC is the time at Greenwich Meridian (GMT)
    CONSOLE.print((epoch  % SECONDS_PER_DAY) / 3600); // print the hour (86400 equals secs per day)
    CONSOLE.print(':');
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      CONSOLE.print('0');
    }
    CONSOLE.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    CONSOLE.print(':');
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      CONSOLE.print('0');
    }
    CONSOLE.println(epoch % 60); // print the second
  }

  return epoch;
}

bool wifiConnect() {
  if (WiFi.status() == WL_CONNECTED) {
    return true; // already connected
  }
  
  CONSOLE.print("  > connecting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(systemSettings.wifi_sta_ssid, systemSettings.wifi_sta_pass);

  long timeoutStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - timeoutStart > CONNECTION_TIMEOUT) {
      CONSOLE.println("  > failed");
      return false;
    }
    delay(100);
    CONSOLE.print(".");
  }
  CONSOLE.println();

  CONSOLE.print("  > connected to "); CONSOLE.println(systemSettings.wifi_sta_ssid);
  digitalWrite(LED_BUILTIN, LOW); // turn LED on
  //if (systemSettings.debug) WiFi.printDiag(CONSOLE);
  
  return true;
}

// send an NTP request to the time server at the given address
void ntpSend(IPAddress& address) {
  if (systemSettings.debug) {
    CONSOLE.print("NTP: sending packet to ");
    CONSOLE.println(address);
  }
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

struct Settings loadSettings() {
  Settings settings; 

  EEPROM.begin(sizeof(Settings));
  EEPROM.get(CONFIG_START, settings);
  
  if (strcmp(settings.version, CONFIG_VERSION) == 0) {
    CONSOLE.print("  > loaded settings data: ");
    printSettingRaw(settings);
    CONSOLE.println();
    EEPROM.end();
    return settings;
  }

  // settings not found, so try to write the defaults
  Settings newSettings;
  CONSOLE.println("  > not found");
  CONSOLE.println("  > writing default data");
  storeSettings(newSettings); // defaults are defined in the structure definition
  return newSettings;
}

void storeSettings(struct Settings settingsIn) {
  noInterrupts();
  struct Settings settings = settingsIn;
  interrupts();

  EEPROM.begin(sizeof(Settings));
  EEPROM.put(CONFIG_START, settings);
  EEPROM.end();
  
  CONSOLE.print("  > stored settings data: ");
  printSettingRaw(settings);
  CONSOLE.println();
}

void printSettingRaw(struct Settings settings) {
  for (unsigned int t = 0; t < sizeof(Settings); t++) {
    CONSOLE.print((char)(*((char*)&settings + t)), HEX); // TODO: fix values < 0x10
    CONSOLE.print(" ");
  }
}

void pulse_ll(bool polarity) {
  if (polarity) {
    digitalWrite(PIN_PULSE_1, HIGH);
  }
  else {
    digitalWrite(PIN_PULSE_2, HIGH);
  }
  delay(200);
  digitalWrite(PIN_PULSE_1, LOW);
  digitalWrite(PIN_PULSE_2, LOW);
}

void pulse() {
  digitalWrite(PIN_LED_STATUS, HIGH);
  // repeat the previous pulse, just for case, that someone tamperred with the manual setting
  // TODO: review this in the future
  pulse_ll(!output_polarity);
  delay(100);
  // make new pulse of required polarity
  pulse_ll(output_polarity);
  
  digitalWrite(PIN_LED_STATUS, LOW);
  output_polarity = !output_polarity;
}
