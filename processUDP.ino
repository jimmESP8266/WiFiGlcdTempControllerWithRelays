/*
WiFiGlcdTempControllerWithRelays - processUDP.ino

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

void processUDP(void)
{
  udpTimeout.detach(); //stop the UDP timer
  if(setDebug & udpDebug)
  {
    Serial.print(millis() / 1000);
    Serial.print(":Packet of ");
    Serial.print(noBytes);
    Serial.print(" received from ");
    Serial.print(Udp.remoteIP());
    Serial.print(":");
    Serial.println(Udp.remotePort(), HEX);
  }
  
  // We've received a packet, read the data from it
  Udp.read(packetBuffer, noBytes); // read the packet into the buffer

  // display the packet contents in HEX
  if(setDebug & udpDebug)
  {
    for (int i = 1;i <= noBytes; i++)
    {
      Serial.print(packetBuffer[i-1]);
      if (i % 32 == 0)
      {
        Serial.println();
      }
    } // end for
    Serial.println();
  }

  switch(packetBuffer[0])
  {
    case 'A':
    {
      if(packetBuffer[1] == 'N')
      {
        digitalWrite(coolingRelay, HIGH);
        packetCnt = sprintf(packetBuffer, "%s", "Turning Cooling Relay ON");
      }else{
        digitalWrite(coolingRelay, LOW);
        packetCnt = sprintf(packetBuffer, "%s", "Turning Cooling Relay OFF");
      }
      break;
    }

    case 'B':
    {
      if(packetBuffer[1] == 'N')
      {
        digitalWrite(heatingRelay, HIGH);
        packetCnt = sprintf(packetBuffer, "%s", "Turning Heating Relay ON");
      }else{
        digitalWrite(heatingRelay, LOW);
        packetCnt = sprintf(packetBuffer, "%s", "Turning Heating Relay OFF");
      }
      break;
    }

    case 'D':
    {
      if(setDebug & udpDebug)
        Serial.print("Setting setDebug to 0x");
      setDebug = atoi(&packetBuffer[1]);
      Serial.println(setDebug, HEX);
      packetCnt = sprintf(packetBuffer, "setDebug = %0X", setDebug);
      break;
    }

    case 'M':
    {
      switch(packetBuffer[1])
      {
        case 'A':
        {
          mode = 'A';
          packetCnt = sprintf(packetBuffer, "Mode is %c - Automatic", mode);
          break;
        }

        case 'M':
        {
          mode = 'M';
          packetCnt = sprintf(packetBuffer, "Mode is %c - Manual", mode);
          break;
        }

        default:
        {
          packetCnt = sprintf(packetBuffer, "Invalid Option - mode is %c", mode);
          break;
        }
      }
      updateEEPROM(EEMode);
      break;
    }

    case 'N':
    {
      uint8_t setDomain = 0, y;

      if((packetBuffer[1] == ' ') && (isalnum(packetBuffer[2])) )
      {
        for(y = 0; y < domainCnt; y++)
        {
          if(isalnum(packetBuffer[y+2]))
          {
            if(setDebug & udpDebug)
              Serial.print(packetBuffer[y+2]);
            continue;
          }else if( ((packetBuffer[y+2] == 0x00) || 
                    (packetBuffer[y+2] == 0x0A) ||
                    (packetBuffer[y+2] == 0x0D)) &&
                    y > 0
                  ){
            if(setDebug & udpDebug)
              Serial.println();
            setDomain = 1;
            break;
          }
        }

        if(setDomain == 1)
        {
          mDNSset = usemDNS;
          for(y = 0; y < domainCnt; y++) // clear domain name buffer
            mDNSdomain[y] = 0;
          for(y = 0; y < domainCnt; y++)
          {
            if(isalnum(packetBuffer[y+2])) // move domain name to domain buffer
              mDNSdomain[y] = packetBuffer[y+2];
          }
          updateEEPROM(EEmDNSset);
          softReset = TRUE;
        }
      }
      packetCnt = sprintf(packetBuffer, "mDNSdomain = %s", mDNSdomain);
      break;
    }

    case 'R':
    {
      if(udpStatusSet == TRUE)
      {
        udpUpdate.detach();
        udpStatus.stop();
      }
      statusIP = Udp.remoteIP();
      statusPort = Udp.remotePort();
//      statusPort = atoi(&packetBuffer[1]);
      udpStatus.begin(statusPort);
      startUdpStatusUpdate();
      packetCnt = sprintf(packetBuffer, "udpStatusUpdate begun at IP %d.%d.%d.%d, port %d", statusIP[0], statusIP[1], statusIP[2], statusIP[3], statusPort);
      udpStatusSet = TRUE;
      break;
    }

/*
  Process: Sets the appropriate temp value and delay for automatic temperature control
  Format:  S,LOC,TYPE,TEMP,DELAY
  Where:
    S     = The UDP command 'S'
    LOC   = The First 'U' or second 'L' relay
    TYPE  = Either Celsius 'C' or Fahrenheit 'F'
    TEMP  = The Temperature in integer degrees, ie "100"
    DELAY = The Delay before activating the relay in seconds, ie "10"
*/
    case 'S':
    {
      char *pos = strchr(packetBuffer, ',');
      char relayLOC   = (char) *(pos+1);
      pos = strchr(pos+1, ',');
      char relayTYPE  = (char) *(pos+1);
      pos = strchr(pos+1, ',');
      char *pos1 = strchr(pos+1, ',');
      *pos1 = 0x00;
      char *relayTEMP  = pos+1;
      char *relayDELAY = pos1+1;

      if(setDebug & udpDebug)
      {
        Serial.print("relayLOC = ");
        Serial.println(relayLOC);
        Serial.print("relayTYPE = ");
        Serial.println(relayTYPE);
        Serial.print("relayTEMP = ");
        Serial.println(relayTEMP);
        Serial.print("relayDELAY = ");
        Serial.println(relayDELAY);
      }
      
      uint8_t   usecool  = 0, useheat = 0, sError = 0;
      int16_t   useTemp   = atoi(relayTEMP);
      uint16_t  useDelay  = atoi(relayDELAY);
      
      switch(relayLOC)
      {
        case 'U':
        {
          usecool = 1;
          relay[coolingRelay].relayDelay = useDelay;
          s0Set = useS0;
          updateEEPROM(EEs0Delay);
          break;
        }

        case 'L':
        {
          useheat = 1;
          relay[heat].relayDelay = useDelay;
          s1Set = useS1;
          updateEEPROM(EEs1Delay);
          break;
        }

        default:
        {
          sError = 1;
          break;
        }
      }
      
      switch(relayTYPE)
      {
        
        case 'C':
        {
          if(useheat == 1)
          {
            heatC = useTemp;
            heatF = (((useTemp * 9) / 5) + 32);
            updateEEPROM(EEheatC);
          }else if(usecool == 1)
          {
            coolC = useTemp;
            coolF = (((useTemp * 9) / 5) + 32);
            updateEEPROM(EEcoolC);
          }else{
            sError = 1;
          }
          break;
        }

        case 'F':
        {
          if(useheat == 1)
          {
            heatF = useTemp;
            heatC = (((useTemp - 32) * 5) / 9); 
            updateEEPROM(EEheatF);
          }else if(usecool == 1)
          {
            coolF = useTemp;
            coolC = (((useTemp - 32) * 5) / 9); 
            updateEEPROM(EEcoolF);
          }else{
            sError = 1;
          }
          break;
        }
        
        default:
        {
          sError = 1;
          break;
        }
      }

      if(sError == 0)
      {
        packetCnt = sprintf(packetBuffer, "coolingRelay is %d F, %d C, %d Sec, heat is %d F, %d C, %d Sec",
                            coolF, coolC, relay[cool].relayDelay,
                            heatF, heatC, relay[heat].relayDelay
                            );
      }else{
        packetCnt = sprintf(packetBuffer, "Invalid Option - coolingRelay value is %d F, %d C, %d Sec, heat is %d F, %d C, %d Sec",
                            coolF, coolC, relay[cool].relayDelay,
                            heatF, heatC, relay[heat].relayDelay
                            );

      }
      break;
    }

    case 'V':
    {
      packetCnt = sprintf(packetBuffer, "%d, %d, %d, %d, %d, %d", 
                          coolF, coolC, relay[cool].relayDelay,
                          heatF, heatC, relay[heat].relayDelay
                         );
      break;
    }
    
    default:
    {
      packetCnt = sprintf(packetBuffer, "%d, %d, %c, %c, %s", ds18b20.tempCelsius, ds18b20.tempFahrenheit,
                                                          relay[cool].relayStatus, relay[heat].relayStatus, mDNSdomain);
      break;
    }
  }

  if(setDebug & udpDebug)
    Serial.println(packetBuffer);
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write(packetBuffer, packetCnt);
  Udp.endPacket();
  udpTimeout.attach_ms(udpDelay, udpReset); //start the UDP timer
}

void udpReset(void)
{
  udpTimeout.detach(); //stop the UDP timer
  Serial.println("UDP Timeout - Resetting");
  ESP.restart();
}
