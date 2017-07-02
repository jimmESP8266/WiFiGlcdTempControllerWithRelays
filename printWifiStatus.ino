/*
WiFiGlcdTempControllerWithRelays - printWifiStatus.ino

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
*/

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void strToIP(uint8_t *ipArray, char *str)
{
  ipArray[0] = (uint8_t) atoi(str);
  ipArray[1] = (uint8_t) atoi((char *) &str[4]);
  ipArray[2] = (uint8_t) atoi((char *) &str[8]);
  ipArray[3] = (uint8_t) atoi((char *) &str[12]);
}

uint8_t startWifi(void)
{
  Serial.println("startWiFi");
  WiFi.mode(WIFI_STA);
  if(wifiVal.useDHCP == FALSE)
  {
    WiFi.config(wifiVal.staticIP, wifiVal.staticGateway, wifiVal.staticSubnet);
  }

  yield();
  
  // Wait for connect to AP
  if(WiFi.begin(wifiVal.ssid, wifiVal.passwd))
  {
    Serial.print("Connecting to ");
    Serial.println(wifiVal.ssid);
    glcd.setRotation(ROTATION);
    yield();
    glcd.fillScreen(ILI9341_BLACK);
    yield();
    glcd.setTextColor(ILI9341_YELLOW);
    yield();
    glcd.setTextSize(3);
    yield();
    glcd.setCursor(0, 0);
    yield();
    xCursor = glcd.getCursorX();
    yCursor = glcd.getCursorY();
    glcd.fillRect(xCursor, yCursor, (int16_t) (320 - xCursor), (int16_t) 21, (uint16_t) ILI9341_BLACK);
    glcd.setCursor(xCursor, yCursor);
    delay(delayVal);
    glcd.println("  CONNECTING TO");
    yield();
    xCursor = glcd.getCursorX();
    yCursor = glcd.getCursorY();
    glcd.fillRect(xCursor, yCursor, (int16_t) (320 - xCursor), (int16_t) 21, (uint16_t) ILI9341_BLACK);
    glcd.setCursor(xCursor, yCursor);
    glcd.print("  ");
    glcd.println(wifiVal.ssid);
    yield();
  }
  int tries=0;
  while (WiFi.status() != WL_CONNECTED)
  {
    startDebugUpdate();
    delay(500);
    yield();
    Serial.print(".");
    tries++;
    if (tries > 30)
    {
      Serial.println();
      Serial.print("Unable to Connect to ");
      Serial.println(wifiVal.ssid);
      yield();
      xCursor = glcd.getCursorX();
      yCursor = glcd.getCursorY();
      glcd.fillRect(xCursor, yCursor, (int16_t) (320 - xCursor), (int16_t) 21, (uint16_t) ILI9341_BLACK);
      glcd.setCursor(xCursor, yCursor);
      delay(delayVal);
      yield();
      glcd.println("UNABLE TO CONNECT");
      yield();
      delay(5000);
      return 0;
    }
    dBug.detach();
  }
  return 1;
}


