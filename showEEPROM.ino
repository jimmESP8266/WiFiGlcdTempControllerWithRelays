/*
WiFiGlcdTempControllerWithRelays - showEEPROM.ino

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

void showEEPROM(void)
{
  EEPROM_readAnything(EEheatC, heatC);
  EEPROM_readAnything(EEheatF, heatF);
  EEPROM_readAnything(EEcoolC, coolC);
  EEPROM_readAnything(EEcoolF, coolF);
  EEPROM_readAnything(EEMode, mode);
  EEPROM_readAnything(EEmDNSset, mDNSset);
  if(mDNSset == usemDNS)  
    EEPROM_readAnything(EEmDNSdomain, mDNSdomain);

  EEPROM_readAnything(EEs0DelaySet, s0Set);
  if(s0Set == useS0)
  {
    EEPROM_readAnything(EEs0Delay, relay[0].relayDelay);
  }  
  EEPROM_readAnything(EEs1DelaySet, s1Set);
  if(s1Set == useS1)
  {
    EEPROM_readAnything(EEs1Delay, relay[1].relayDelay);
  }  

  if(setDebug & eepromDebug)
  {
    Serial.print("heatC = ");
    Serial.print(heatC);
    Serial.print(", heatF = ");
    Serial.print(heatF);
    Serial.print(", coolC = ");
    Serial.print(coolC);
    Serial.print(", coolF = ");
    Serial.println(coolF);
    Serial.print("relay[0].relayDelay = ");
    Serial.print(relay[0].relayDelay);
    Serial.print(", relay[1].relayDelay = ");
    Serial.println(relay[1].relayDelay);
    Serial.print("mode = ");
  
    switch(mode)
    {
      case 'A':
      {
        Serial.print("Automatic");      
        break;
      }
      
      case 'M':
      {
        Serial.print("Manual");      
        break;
      }
      
      default:
      {
        Serial.print("Uninitialized");      
        break;
      }
    }
    Serial.println();
    delay(100);
    Serial.print("mDNSdomain = ");
    Serial.println(mDNSdomain);
    delay(100);
/*
    Serial.print("ssid = ");
    Serial.println(ssid);
    delay(100);
    Serial.print("passwd = ");
    Serial.println(passwd);
    delay(100);
    Serial.print("staticIP = ");
    Serial.println(staticIP);
    delay(100);
    Serial.print("staticGateway = ");
    Serial.println(staticGateway);
    delay(100);
    Serial.print("staticSubnet = ");
    Serial.println(staticSubnet);
    delay(100);

    Serial.print("udpPort = ");
    Serial.println(udpPort);
*/
  }

}

void updateEEPROM(uint16_t level)
{
  switch(level)
  {
    case EEheatC:
    case EEheatF:
    {
      EEPROM_writeAnything(EEheatC, heatC);
      EEPROM_writeAnything(EEheatF, heatF);
      break;
    }
    
    case EEcoolC:
    case EEcoolF:
    {
      EEPROM_writeAnything(EEcoolC, coolC);
      EEPROM_writeAnything(EEcoolF, coolF);
      break;
    }
    
    case EEMode:
    {
      EEPROM_writeAnything(EEMode, mode);
      break;
    }

    case EEmDNSset:
    {
      EEPROM_writeAnything(EEmDNSset, mDNSset);
      EEPROM_writeAnything(EEmDNSdomain, mDNSdomain);
      break;
    }

    case EEs0Delay:
    {
      EEPROM_writeAnything(EEs0DelaySet, s0Set);
      EEPROM_writeAnything(EEs0Delay, relay[0].relayDelay);
      break;
    }

    case EEs1Delay:
    {
      EEPROM_writeAnything(EEs1DelaySet, s1Set);
      EEPROM_writeAnything(EEs1Delay, relay[1].relayDelay);
      break;
    }
  }
  EEPROM.commit();
  showEEPROM();
}
