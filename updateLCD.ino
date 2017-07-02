/*
WiFiGlcdTempControllerWithRelays - updateLCD.ino

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.0
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

void setLcdStatus(void)
{
  if(setDebug & lcdDebug)
  {
    Serial.println("Setting lcdSetStatus");
  }
  lcd.attach(lcdDelayVal, lcdSetStatus);
  lcdStatus = FALSE;
}

void lcdSetStatus(void)
{
  lcd.detach();
  lcdStatus = TRUE;
}


void updateLCD(void)
{
//  int16_t xCursor, yCursor;

  if(setDebug & lcdDebug)
  {
    Serial.println("Time to Write to the LCD");
  }
  
  glcd.setRotation(ROTATION);
  yield();
  glcd.setTextColor(ILI9341_YELLOW);
  yield();
  glcd.setTextSize(3);
  yield();
  glcd.setCursor(0, 0);

  if(setDebug & lcdDebug)
  {
    Serial.println("Writing to tft");
  }

  IPAddress ip = WiFi.localIP();
  sprintf(lcdBuffer, "IP:%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (320 - xCursor), 21, ILI9341_BLACK);
  glcd.setCursor(xCursor, yCursor);
  glcd.println(lcdBuffer);
  delay(delayVal);
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (320 - xCursor), 21, ILI9341_BLACK);
  glcd.setCursor(xCursor, yCursor);
  glcd.println(versionInfo);
  delay(delayVal);
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (320 - xCursor), 21, ILI9341_BLACK);
  glcd.setCursor(xCursor, yCursor);
  glcd.print("Port: ");
  glcd.print(wifiVal.udpPort);
  delay(delayVal);
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (320 - (xCursor + 10)), 21, ILI9341_BLACK);
  glcd.setCursor(xCursor + 10, yCursor);
  switch(mode)
  {
    case 'A':
    {
      glcd.println("Auto");      
      break;
    }
    
    case 'M':
    {
      glcd.println("Man");      
      break;
    }
    
    default:
    {
      glcd.println("None");      
      break;
    }
  }
  delay(delayVal);
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (320 - xCursor), 21, ILI9341_BLACK);
  glcd.setCursor(xCursor, yCursor);
  glcd.println(mDNSdomain);

  glcd.setTextSize(4);
  delay(delayVal);
  sprintf(lcdBuffer, "Temp: %d F", ds18b20.tempFahrenheit);
  glcd.print("Temp: ");
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (320 - xCursor), 28, ILI9341_BLACK);
  glcd.setCursor(0, yCursor);
  glcd.println(lcdBuffer);
  if(setDebug & lcdDebug)
  {
    Serial.println(lcdBuffer);
  }
  delay(delayVal);
  sprintf(lcdBuffer, "Temp: %d C", ds18b20.tempCelsius);
  glcd.print("Temp: ");
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (320 - xCursor), 28, ILI9341_BLACK);
  glcd.setCursor(0, yCursor);
  glcd.println(lcdBuffer);
  if(setDebug & lcdDebug)
  {
    Serial.println(lcdBuffer);
  }
  delay(delayVal);
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (320 - xCursor), 28, ILI9341_BLACK);
  glcd.setCursor(0, yCursor);
  if(relay[0].relayStatus == 'F')
  {
    glcd.println("COOLING: OFF");
  }else{
    glcd.println("COOLING: ON");
  }
  delay(delayVal);
  xCursor = glcd.getCursorX();
  yCursor = glcd.getCursorY();
  glcd.fillRect(xCursor, yCursor, (320 - xCursor), 28, ILI9341_BLACK);
  glcd.setCursor(0, yCursor);
  if(relay[1].relayStatus == 'F')
  {
    glcd.println("HEATING: OFF");
  }else{
    glcd.println("HEATING: ON");
  }
  delay(delayVal);
  if(setDebug & lcdDebug)
  {
    Serial.println(lcdBuffer);
    Serial.println("Finished writing to GLCD");
  }
  setLcdStatus();
}
