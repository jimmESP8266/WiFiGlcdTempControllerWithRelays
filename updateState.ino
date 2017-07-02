/*
WiFiGlcdTempControllerWithRelays - updateState.ino

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

void updateState(void)
{
  if(setDebug & relayDebug)
    Serial.println("Entering updateState()");
  if(mode == 'A')
  {
    if(ds18b20.tempFahrenheit >= coolF)
    {
      if(relay[cool].relayDelay == 0)
      {
        digitalWrite(coolingRelay, HIGH); // cool things off
        relay[cool].relayStatus = 'N';
        if(setDebug & relayDebug)
          Serial.println("Set cool relay immediate- ON");
      }else if(relay[cool].relayDelaySet == FALSE){
        if(setDebug & relayDebug)
        {
          Serial.print("set coolDelay = ");
          Serial.println(relay[cool].relayDelay);
        }
        relay[cool].relayDelaySet = TRUE;
        cooling_relay.attach_ms((relay[cool].relayDelay * 1000), startcool);
        startCoolTimer = millis();
      }
    }else{
      if(setDebug & relayDebug)
        Serial.println("Set cool - OFF, ");
      digitalWrite(coolingRelay, LOW);
      relay[cool].relayStatus = 'F';
    }

    if(ds18b20.tempFahrenheit <= heatF)
    {
      if(relay[heat].relayDelay == 0)
      {
        digitalWrite(heatingRelay, HIGH); // heat things up
        relay[heat].relayStatus = 'N';
        if(setDebug & relayDebug)
          Serial.println("Set heat relay immediate - ON");
      }else if(relay[heat].relayDelaySet == FALSE){
        if(setDebug & relayDebug)
        {
          Serial.print("set heatDelay = ");
          Serial.println(relay[heat].relayDelay);
        }
        relay[heat].relayDelaySet = TRUE;
        heating_relay.attach_ms((relay[heat].relayDelay * 1000), startheat);
        startHeatTimer = millis();
      }
    }else{
      if(setDebug & relayDebug)
        Serial.println("heat - OFF");
      digitalWrite(heatingRelay, LOW);
      relay[heat].relayStatus = 'F';
    }
  }
  if(setDebug & relayDebug)
    Serial.println("Exiting updateState()");
}

void startcool(void)
{
  cooling_relay.detach();
  if(ds18b20.tempFahrenheit >= coolF)
  {
    digitalWrite(coolingRelay, HIGH); // cool things off
    relay[cool].relayStatus = 'N';
  }else{
    digitalWrite(coolingRelay, LOW);
    relay[cool].relayStatus = 'F';
  }
  relay[cool].relayDelaySet = FALSE;
  if(setDebug & relayDebug)
  {
    Serial.printf("set cool relay ON after a ");
    Serial.print(millis() - startCoolTimer);
    Serial.println(" delay");
  }
}

void startheat(void)
{
  heating_relay.detach();
  if(ds18b20.tempFahrenheit <= heatF)
  {
    digitalWrite(heatingRelay, HIGH); // heat things up
    relay[heat].relayStatus = 'N';
  }else{
    digitalWrite(heatingRelay, LOW);
    relay[heat].relayStatus = 'F';
  }
  relay[heat].relayDelaySet = FALSE;
  if(setDebug & relayDebug)
  {
    Serial.printf("set heat relay ON after a ");
    Serial.print(millis() - startHeatTimer);
    Serial.println(" delay");
  }
}
