/*
   Sensors2Cloud Arduino Monitoring 
   based on: Web client sketch for IDE v1.0.1 and w5100/w5200
   Uses POST method.
   Posted November 2012 by SurferTim
   Modified by Sensors2Cloud Inc.,
   THIS SOFTWARE IS PROVIDED TO YOU "AS IS," AND WE MAKE NO EXPRESS OR IMPLIED WARRANTIES 
   WHATSOEVER WITH RESPECT TO ITS FUNCTIONALITY,   OPERABILITY, OR USE, INCLUDING, WITHOUT LIMITATION, 
   ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR INFRINGEMENT. 
   WE EXPRESSLY DISCLAIM ANY LIABILITY WHATSOEVER FOR ANY DIRECT, INDIRECT, CONSEQUENTIAL, INCIDENTAL 
   OR SPECIAL DAMAGES, INCLUDING, WITHOUT LIMITATION, LOST REVENUES, LOST PROFITS, LOSSES RESULTING 
   FROM BUSINESS INTERRUPTION OR LOSS OF DATA, REGARDLESS OF THE FORM OF ACTION OR LEGAL THEORY 
   UNDER WHICH THE LIABILITY MAY BE ASSERTED, EVEN IF ADVISED OF THE POSSIBILITY OR LIKELIHOOD OF SUCH DAMAGES.
*/

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {  0xAA, 0x00, 0x00, 0x00, 0x00, 0x03 }; // Add Mac Addr of your Arduino Here

//Change to your server domain
//char serverName[] = "146.148.86.197";
char serverName[] = "developer.sensors2cloud.com";
// change to your server's port
int serverPort = 80;

// Analog Ports to read
int analogPin0 = A0;    // select the input pin for the potentiometer
int analogPin1 = A1;    // select the input pin for the potentiometer
int analogPin2 = A2;    // select the input pin for the potentiometer
int analogPin3 = A3;    // select the input pin for the potentiometer
int analogPin4 = A4;    // select the input pin for the potentiometer
int analogPin5 = A5;    // select the input pin for the potentiometer

int analogVal0 = 0;  // variable to store the value coming from the sensor
int analogVal1 = 0;  // variable to store the value coming from the sensor
int analogVal2 = 0;  // variable to store the value coming from the sensor
int analogVal3 = 0;  // variable to store the value coming from the sensor
int analogVal4 = 0;  // variable to store the value coming from the sensor
int analogVal5 = 0;  // variable to store the value coming from the sensor
// change to the page on that server
char pageName[] = "/post";

EthernetClient client;
int totalCount = 0; 
// insure params is big enough to hold your variables
char params[512];
char chanBuff[65];
//char securekey[] = "agtzfm0ybTJjbG91ZHISCxIFQWRtaW4iB2VzbGliZW4M";
char securekey[] = "1AB!456";           // Add your API key Here
char macaddr[]   = "AA:00:00:00:00:03"; // Add Mac Addr of your Arduino Here. Please Follow the format
// set this to the number of milliseconds delay
// this is 3 seconds
#define delayMillis 30000UL
unsigned long thisMillis = 0;
unsigned long lastMillis = 0;

void setup() {
  Serial.begin(9600);

  // disable SD SPI
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);

  Serial.print(F("Starting ethernet..."));
  if(!Ethernet.begin(mac)) Serial.println(F("failed"));
  else Serial.println(Ethernet.localIP());

  delay(2000);
  Serial.println(F("Ready"));
}

void loop()
{
  // If using a static IP, comment out the next line
  Ethernet.maintain();

  thisMillis = millis();

  if(thisMillis - lastMillis > delayMillis)
  {
    lastMillis = thisMillis;
    analogVal0 = analogRead(analogPin0);
    analogVal1 = analogRead(analogPin1);
    analogVal2 = analogRead(analogPin2);
    analogVal3 = analogRead(analogPin3);
    analogVal4 = analogRead(analogPin4);
    analogVal5 = analogRead(analogPin5);
    // params must be url encoded.
      // JSON starts here
      sprintf(params,"%s","{"); 
      sprintf(params,"%s\"macaddr\":\"%s\"",params,macaddr);
      sprintf(params,"%s,\"uptime\":%i",params,55); 
      sprintf(params,"%s,\"sensorList\":%s",params,"[{");
      sprintf(params,"%s\"name\":%s",params,"\"test\""); 
      sprintf(params,"%s,\"channels\":%s",params,"[");
      // AIN0 channel
      sprintf(chanBuff,"{\"name\":\"%s\",\"value\":\"%i\",\"unit\":\"%s\",\"time\":\"%i\"}","AIN0",analogVal0,"V",totalCount);
      sprintf(params,"%s%s,",params,chanBuff);
      // AIN1 channel
      sprintf(chanBuff,"{\"name\":\"%s\",\"value\":\"%i\",\"unit\":\"%s\",\"time\":\"%i\"}","AIN1",analogVal1,"V",totalCount);
      sprintf(params,"%s%s,",params,chanBuff);
      // AIN2 channel
      sprintf(chanBuff,"{\"name\":\"%s\",\"value\":\"%i\",\"unit\":\"%s\",\"time\":\"%i\"}","AIN2",analogVal2,"V",totalCount);
      sprintf(params,"%s%s,",params,chanBuff);
      // AIN3 channel
      sprintf(chanBuff,"{\"name\":\"%s\",\"value\":\"%i\",\"unit\":\"%s\",\"time\":\"%i\"}","AIN3",analogVal3,"V",totalCount);
      sprintf(params,"%s%s,",params,chanBuff);
      // AIN4 channel
      sprintf(chanBuff,"{\"name\":\"%s\",\"value\":\"%i\",\"unit\":\"%s\",\"time\":\"%i\"}","AIN4",analogVal4,"V",totalCount);
      sprintf(params,"%s%s,",params,chanBuff); 
       // AIN5 channel
      sprintf(chanBuff,"{\"name\":\"%s\",\"value\":\"%i\",\"unit\":\"%s\",\"time\":\"%i\"}","AIN5",analogVal5,"V",totalCount);
      sprintf(params,"%s%s",params,chanBuff);     // Notice in the last channel, the %s%s without the comma at the end
      // Complete JSON
      sprintf(params,"%s%s",params,"]}]}");    
   
   
    if(!postPage(serverName,serverPort,pageName,params)) Serial.print(F("Fail "));
    else Serial.print(F("Pass "));
    totalCount++;
    Serial.println(totalCount,DEC);
    Serial.print(params);
  }    
}



byte postPage(char* domainBuffer,int thisPort,char* page,char* thisData)
{
  int inChar;
  char outBuf[64];

  Serial.print(F("connecting..."));

  if(client.connect(domainBuffer,thisPort) == 1)
  {
    Serial.println(F("connected"));

    // send the header
    sprintf(outBuf,"POST %s HTTP/1.1",page);
    client.println(outBuf);
    sprintf(outBuf,"Host: %s",domainBuffer);
    client.println(outBuf);
    sprintf(outBuf,"securekey: %s",securekey);
    client.println(outBuf);
    sprintf(outBuf,"macaddr: %s",macaddr);
    client.println(outBuf);     
    client.println(F("Connection: keep-alive\r\nContent-Type: application/x-www-form-urlencoded"));
    sprintf(outBuf,"Content-Length: %u\r\n",strlen(thisData));
    client.println(outBuf);

    // send the body (variables)
    client.print(thisData);
  } 
  else
  {
    Serial.println(F("failed"));
    return 0;
  }

  int connectLoop = 0;

  while(client.connected())
  {
    while(client.available())
    {
      inChar = client.read();
      Serial.write(inChar);
      connectLoop = 0;
    }

    delay(1);
    connectLoop++;
    if(connectLoop > 10000)
    {
      Serial.println();
      Serial.println(F("Timeout"));
      client.stop();
    }
  }

  Serial.println();
  Serial.println(F("disconnecting."));
  client.stop();
  return 1;
}
