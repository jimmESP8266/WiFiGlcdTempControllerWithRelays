/*
WiFiGlcdTempControllerWithRelays

Version 0.0.1
Last Modified 7/1/2017
By Jim Mayhugh


Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

This software uses multiple libraries that are subject to additional
licenses as defined by the author of that software. It is the user's
and developer's responsibility to determine and adhere to any additional
requirements that may arise.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Configuration :
  Enter the ssid and password of your Wifi AP.
  Enter the port number your server is listening on.

*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Ticker.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <EEPROMAnything.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*************************************************************************
***  These parameters are set to the static variables you wish to be used
**************************************************************************/
const IPAddress sIP = {255,255,255,255}; // change this to the static IP address you wish to use
const IPAddress sGW = {255,255,255,255};   // change this to the static Gateway address you wish to use
const IPAddress sSN = {255,255,255,255}; // change this to the static SubNet address you wish to use

char *cPW = "xxxxxxxxxxx";
char *cIP = "255.255.255.255.255"; // change this to the static IP address you wish to use
char *cGW = "255.255.255.255.255";   // change this to the static Gateway address you wish to use
char *cSN = "255.255.255.255.255"; // change this to the static SubNet address you wish to use
const uint16_t cUB = xxxxxxxx;   // change this to the static SubNet address you wish to use
/*****************************************************************************
***  The above parameters are set to the static variables you wish to be used
******************************************************************************/

int status = WL_IDLE_STATUS;
const uint8_t domainCnt = 15;
const uint8_t bonjourCnt = 50;
const uint8_t usemDNS = 0xAA;

const uint16_t udpDebug       = 0x0001;
const uint16_t tempDebug      = 0x0002;
const uint16_t relayDebug     = 0x0004;
const uint16_t lcdDebug       = 0x0008;
const uint16_t eepromDebug    = 0x0010;
const uint16_t findChipsDebug = 0x0020;
const uint16_t mdnsDebug      = 0x0040;
const uint16_t dBugDebug      = 0x0080;
const uint16_t loopDebug      = 0x0100;

uint16_t setDebug = 0x0020;

char packetBuffer[512]; // buffer to hold incoming and outgoing packets
char updateBuffer[128]; // buffer to hold updateStatus
char lcdBuffer[21];

char *versionInfo = (char *)"Version 0.0.1";

int16_t noBytes, packetCnt;
const uint32_t delayVal = 10, sDelayVal = 500, uDelayVal = 60, lcdDelayVal = 2;
const uint32_t dBugDelay = 100, cDelayVal = 250, udpDelay = (1*60*1000);
int16_t heatC, heatF, coolC, coolF;
uint32_t lowerDelay, coolDelay, tempDelayStart, tempDelayEnd, startCoolTimer, startHeatTimer;
uint32_t loopStart, loopEnd, loopCnt;
int8_t i;
uint8_t udpAddr[5];
uint8_t data[15];
uint8_t chip[8];
uint8_t chipStatus[3];
// char *delim = (char *)",";
// char *result = NULL;
char mDNSdomain[domainCnt] = "ESP8266";
char bonjourBuf[bonjourCnt] = "";
uint8_t chipCnt = 0;
uint8_t mode = 0xFF, mDNSset;

// EEPROM Storage locations
const uint16_t EEPROMsize   = 4096;
const uint16_t EEMode       = 0x0008; // 'M' = Manual, 'A' = Automatic, anything else = uninitialized
const uint16_t EEheatC      = 0x0010;
const uint16_t EEheatF      = 0x0020;
const uint16_t EEcoolC      = 0x0030;
const uint16_t EEcoolF      = 0x0040;
const uint16_t EEmDNSset    = 0x0050; // 0xAA = set, anything else is uninitialized
const uint16_t EEmDNSdomain = 0x0060; // mDNS domain name
const uint16_t EEuseUDPport = 0x00E0; // 0xAA = set, anything else is uninitialized
const uint16_t EEudpPort    = 0x00F0; // UDP port address
const uint16_t EEs0DelaySet = 0x0100; // 0xAA = set, anything else is uninitialized
const uint16_t EEs0Delay    = 0x0110; // Switch1 Delay
const uint16_t EEs1DelaySet = 0x0120; // 0xAA = set, anything else is uninitialized
const uint16_t EEs1Delay    = 0x0130; // Switch1 Delay

const uint8_t useS0 = 0xAA;
const uint8_t useS1 = 0xAA;
uint8_t s0Set = 0, s1Set = 0;

// WiFi stuff
const uint8_t WiFiStrCnt = 32;  // max string length
const uint8_t ipStrCnt = 20;
// const uint8_t useWiFi = 0xAA;
const uint8_t useUDPport = 0xAA;
const uint8_t udpPortCnt = 4;
const uint8_t macCnt = 6;
uint8_t wifiSet = 0, udpSet = 0;
uint8_t macAddress[macCnt] = {0,0,0,0,0,0};

const uint8_t useStaticIP = 0xAA;
const uint8_t useDHCP = 0x55;
uint8_t staticIPset = 0;
uint8_t ipArray[4] = {0 ,0, 0, 0};
uint8_t gwArray[4] = {0 ,0, 0, 0};
uint8_t snArray[4] = {0 ,0, 0, 0};

IPAddress staticIP(255,255,255,255);
IPAddress staticGateway(255,255,255,255);
IPAddress staticSubnet(255, 255, 255, 255);

// ILI9341 GLCD -Ticker  lcd; For the ESP8266, these are the default.
#define TFT_DC 5
#define TFT_CS 15
#define ROTATION 1

int16_t xCursor, yCursor;
//uint8_t lcdCnt = 0;
//const uint8_t updateLCDcnt = 25;

Adafruit_ILI9341 glcd = Adafruit_ILI9341(TFT_CS, TFT_DC);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

// A UDP instance to allow interval updates to be sent
IPAddress statusIP;
uint16_t  statusPort;
WiFiUDP udpStatus;

// OneWire Stuff
// Family codes
const uint8_t t3tcID         = 0xAA; // Teensy 3.0 1-wire slave with MAX31855 K-type Thermocouple chip
const uint8_t dsLCD          = 0x47; // Teensy 3.x 1-wire slave 4x20 HD44780 LCD
const uint8_t dsGLCDP        = 0x45; // Teensy 3.1 1-wire slave 800x400 7" GLCD with Paging
const uint8_t dsGLCD         = 0x44; // Teensy 3.1 1-wire slave 800x400 7" GLCD
const uint8_t max31850ID     = 0x3B; // MAX31850 K-type Thermocouple chip
const uint8_t ds2762ID       = 0x30; // Maxim 2762 digital k-type thermocouple
const uint8_t ds18b20ID      = 0x28; // Maxim DS18B20 digital Thermometer device
const uint8_t ds2406ID       = 0x12; // Maxim DS2406+ digital switch

// DS2406+ Digital Switch Family Code and registers
const uint8_t dsPIO_A        = 0x20;
const uint8_t dsPIO_B        = 0x40;
const uint8_t ds2406MemWr    = 0x55;
const uint8_t ds2406MemRd    = 0xaa;
const uint8_t ds2406AddLow   = 0x07;
const uint8_t ds2406AddHi    = 0x00;
const uint8_t ds2406PIOAoff  = 0x3f;
const uint8_t ds2406PIOAon   = 0x1f;
const uint8_t ds2406End      = 0xff;

const uint8_t switchStatusON   = 'N';
const uint8_t switchStatusOFF  = 'F';

const uint8_t oneWireAddress  =   2; // OneWire Bus Address

const uint8_t chipAddrSize    =   8; // 64bit OneWire Address
const uint8_t tempDataSize    =   9; // temp data
const uint8_t switchDataSize  =  13; // switch data
const uint8_t chipNameSize    =  15;

bool  tempConversion = FALSE;
bool  sendStatus = FALSE;
bool  softReset = FALSE;
bool  udpStatusSet = FALSE;
bool  mdnsUpdateStatus = FALSE;
bool  lcdStatus = FALSE;

// Temp Stuff
typedef struct
{
  uint8_t     tempAddr[chipAddrSize];
  uint8_t     tempData[tempDataSize];
  int16_t     tempFahrenheit;
  int16_t     tempCelsius;
}tempStruct;

const tempStruct tempClear = { {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0, 0};

tempStruct ds18b20 = { {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0}, 0, 0};

// SwitchStuff
const uint8_t maxRelays = 2;
const uint8_t coolingRelay = 0, cool = 0, heatingRelay = 4, heat = 1;

typedef struct
{
  uint8_t     relay;
  char        relayStatus;
  uint32_t    relayDelay;
  bool        relayDelaySet;
}relayStruct;

const relayStruct relayClear = { 0xff, 'F', 0, FALSE};

relayStruct relay[maxRelays] =
{
  { coolingRelay, 'F', 0, FALSE},
  { heatingRelay, 'F', 0, FALSE}
};

OneWire ds(oneWireAddress);  // on pin 2 (a 4.7K resistor is necessary)
Ticker  ds18; // timer to allow DS18B20 to be read
Ticker  cooling_relay;
Ticker  heating_relay;
Ticker  udpUpdate;
Ticker  mdnsServer;
Ticker  lcd;
Ticker  dBug;
Ticker  udpTimeout;

const uint8_t maxWiFi = 50; 
uint8_t useWiFi = 0;

typedef struct
{
  char  *ssid;        // your network SSID (name)
  char  *passwd;      // your network password
  bool  useDHCP;      // true = use DHCP, FALSE = use Static IP
  char  *ipBuf;
  char  *gwBuf;
  char  *snBuf;
  uint16_t udpPort;                // local port to listen for UDP packets
  IPAddress staticIP;
  IPAddress staticGateway;
  IPAddress staticSubnet;
}wifiStruct;

wifiStruct wifiVal = {"GMJLinksys2", cPW, TRUE, cIP, cGW, cSN, cUB, sIP, sGW, sSN};

// mDNS stuff
// multicast DNS responder
MDNSResponder mdns;
uint16_t mdnsUpdateDelay = 10;
char bonjourNameBuf[25];


void setup(void)
{
  
  ESP.wdtDisable(); // disable the watchdog Timer

  // set relay pins
  pinMode(coolingRelay, OUTPUT);
  pinMode(heatingRelay, OUTPUT);
  digitalWrite(coolingRelay, LOW);
  digitalWrite(heatingRelay, LOW);

  EEPROM.begin(EEPROMsize);
  // Open serial communications and wait for port to open:
  showEEPROM();
  
  Serial.begin(115200);
  delay(sDelayVal);
  Serial.println();
  delay(sDelayVal);
  Serial.println();
  delay(sDelayVal);

  Serial.println("Initialize GLCD");
  glcd.begin();
  glcd.setRotation(ROTATION);
  yield();
  glcd.fillScreen(ILI9341_BLACK);
  yield();
  glcd.setTextColor(ILI9341_YELLOW);
  yield();
  glcd.setTextSize(3);
  yield();
  glcd.setCursor(0, 0);
  glcd.println("  INITIALIZING  ");
  yield();
  glcd.println("  PLEASE WAIT   ");
  yield();

  Serial.print("MAC Address = ");
  uint8_t *mac = WiFi.macAddress(macAddress);
  for(uint8_t q = 0; q < macCnt; q++)
  {
    if(mac[q] < 0x10)
      Serial.print("0");
    Serial.print(mac[q], HEX);
    if(q < (macCnt - 1))
      Serial.print(":");
  }
  Serial.println();

  do
  {
    if(startWifi())
    {
      break;
    }else{
      useWiFi++;
    }
  }while(useWiFi < maxWiFi);
  
  if(useWiFi >= maxWiFi)
  {
    Serial.println("WiFiSetup failed");
    yield();
    xCursor = glcd.getCursorX();
    yCursor = glcd.getCursorY();
    glcd.fillRect(xCursor, yCursor, (int16_t) (320 - xCursor), (int16_t) 21, (uint16_t) ILI9341_BLACK);
    glcd.setCursor(xCursor, yCursor);
    delay(delayVal);
    glcd.println("WIFI Setup FAILED");
    yield();
    xCursor = glcd.getCursorX();
    yCursor = glcd.getCursorY();
    glcd.fillRect(xCursor, yCursor, (int16_t) (320 - xCursor), (int16_t) 21, (uint16_t) ILI9341_BLACK);
    glcd.setCursor(xCursor, yCursor);
    delay(delayVal);
    glcd.println("    RESETTING    ");
    delay(10000);
    ESP.restart();
  }else{
    Serial.print("Using Static IP - ");
    IPAddress ip = WiFi.localIP();
    Serial.print("Connected to wifi ");
    Serial.println(wifiVal.ssid);
    Serial.print("Connected to wifi at IP Address: ");
    Serial.println(ip);
    if(mDNSset != 0xAA)
    {
      sprintf(mDNSdomain, "%s%d", mDNSdomain, ip[3]);
    }
  }

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network

  if (mdns.begin(mDNSdomain, WiFi.localIP()))
  {
    startMDNSupdate();
    Serial.print("mDNS responder started with ");
    Serial.println(mDNSdomain);
    mdns.addService("esp", "tcp", 8080); // Announce esp tcp service on port 8080  
  }else{
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }


  if(setDebug > 0)
  {
    printWifiStatus();
  }

  Serial.print("Udp server started at port at ");
  Serial.println(wifiVal.udpPort);
  Udp.begin(wifiVal.udpPort);
  yield();
  setLcdStatus();
  yield();
  findChips();
  yield();
  scanChips();
  loopCnt = 0;
  udpTimeout.attach_ms(udpDelay, udpReset); //start the UDP timer
}

void loop(void)
{
  startDebugUpdate();
  noBytes = Udp.parsePacket();
  loopStart = millis();
  yield();

  if(tempConversion == TRUE)
  {
    if(setDebug & tempDebug)
      Serial.println("tempConversion is TRUE");
    readTemp();
    yield();
    updateState();
    yield();
    scanChips();
    loopCnt = 0;
  }else{
    if(setDebug & tempDebug)
      Serial.println("tempConversion is FALSE");
  }

  yield();
  if(lcdStatus == TRUE)
    updateLCD();

  yield();
  if ( noBytes )
    processUDP();

  yield();
  if ( sendStatus == TRUE )
  {
    statusUpdate();
    sendStatus = FALSE;
  }

  yield();
  if ( mdnsUpdateStatus == TRUE )
    startMDNSupdate();

  delay(10);
  loopEnd = millis();
  if(setDebug & loopDebug)
  {
    Serial.print("loopStart = ");
    Serial.println(loopStart);
    Serial.print("loopEnd = ");
    Serial.println(loopEnd);
    Serial.print("Total loopDelay = ");
    Serial.println(loopEnd - loopStart);
    Serial.print("loopCnt = ");
    Serial.println(loopCnt);
  }
  loopCnt++;
  dBug.detach();
}


