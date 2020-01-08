/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "application.h"
#line 1 "c:/Users/ryan.trisnojoyo/Documents/Particle/projects/Rain_Gauge/src/RainGaugeTest/src/RainGaugeTest.ino"


void setup();
void myHandler(const char *event, const char *data);
void parseString(String strVars);
bool setParameter(String param, String value);
void statusMessage();
void loop();
void initSyncTime();
void initConnection();
void disconnectConnection();
void calculateRainGaugeData();
#line 3 "c:/Users/ryan.trisnojoyo/Documents/Particle/projects/Rain_Gauge/src/RainGaugeTest/src/RainGaugeTest.ino"
int led1 = D0; // Instead of writing D0 over and over again, we'll write led1
// You'll need to wire an LED to this one to see it blink.

int led2 = D7; // Instead of writing D7 over and over again, we'll write led2
// This one is the little blue LED on your board. On the Photon it is next to D7, and on the Core it is next to the USB jack.

const int buttonPin = D4;

int sendInterval = 12; // number of samples collected to transmit
int  logInterval = 5;  // Set the log interval in minutes 
int statusInterval = 1440; //  Status message interval in minutes (default to 1 day)
long int lastStatusMessage = 0;
long int prevTime;
long int currentTime;
int scheduledWakeup = 0;
int tripCount = 0;         /******* Use descriptive variable names, not 'i' (FOR loops OK)  ***********/
int samplesLogged = 0;
int buttonState = 0;
char payload[128];
char totalPayload[128];

PRODUCT_ID(10618);
PRODUCT_VERSION(1.0);

SYSTEM_MODE (MANUAL);

void setup()
{ 
  delay(3000);
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  initSyncTime();
  prevTime = Time.now();          /***** Use full Epoch time, not minutes  ******/

  Particle.subscribe(System.deviceID() + "/hook-response/Status", myHandler, MY_DEVICES);

  Serial.println("Starting");
}
void myHandler(const char *event, const char *data) {
  Serial.println("response received");
  parseString(data);
}
       
void parseString(String strVars)
{

  strVars = strVars.replace("\"","");
  String parameter = ""; 
  String value = "";
  String inChar = "";
  int i = 0;
  bool readingParam = true;

  while (inChar != ".")
  {
  inChar = strVars.charAt(i++);
  if (inChar != ":" and inChar != ",")
    {
    if (readingParam)
      parameter += inChar;
    else
      value += inChar;    
    }
  else
    {
      if (inChar == ":")
      {
        readingParam = false;
      }
      else
      {
        if (setParameter(parameter, value) == false)
          return;
        parameter = "";
        value = ""; 
        readingParam = true;
      }
    }
  }
  setParameter(parameter,value.replace(".",""));
}
bool setParameter(String param, String value)
{
if (param == "si")
  {
      Serial.println("Setting sendInterval to: " + value);
      sendInterval = value.toInt();
  }
  else if (param == "li")
  {
      Serial.println("Setting logInterval to: " + value);
      logInterval = value.toInt();
  }
  else if (param == "sm")
  {
      Serial.println("Setting statusInterval to: " + value);
      statusInterval = value.toInt();
  }
  else
  {
      Serial.println("Unknown parameter- " + param + ":" + value);
  }
  
  return true;
}

/****  Every version of code should contain a daily status message and wait for response  ****/
void statusMessage()
{
  initConnection();
  String message =  String(Time.now()) + "," + "Status,li:" + (String)logInterval + ",si:" + (String)sendInterval + ",sm:" + (String)statusInterval;
  Particle.publish("Status", message, PRIVATE);
  lastStatusMessage = Time.now();
  delay(10000);
  disconnectConnection();

}

void loop()
{
  currentTime = Time.now();
  buttonState = digitalRead(buttonPin);

  /******* Check if status message is due  *****/
  if (lastStatusMessage + (60 * statusInterval) < Time.now() )
  {
    statusMessage();
  }

  if ((currentTime - prevTime >= (logInterval * 60)) && scheduledWakeup)
  {

    strcat(payload,",");
    strcat(payload,String(tripCount));

    Serial.print("Payload:");
    Serial.println(payload);
    Serial.println(prevTime);
     
    samplesLogged++;
    tripCount = 0;
    prevTime = currentTime;

    if (samplesLogged >= sendInterval)
    {
      strcpy(totalPayload,String(Time.now()-(60*sendInterval*samplesLogged)));
      strcat(totalPayload,",1043,");
      strcat(totalPayload,String(logInterval));
      strcat(totalPayload,payload);
      initConnection();
      Particle.publish("Counter", totalPayload, PRIVATE);
      Serial.println("totalPayload: " + (String)totalPayload);
      disconnectConnection();
      strcpy(payload,"");
      samplesLogged = 0;
      scheduledWakeup = 0;
    }    
  }
  else
  {
    if (!scheduledWakeup)
    {
      disconnectConnection();
      Serial.println("Sleep");
      Serial.println("");
      System.sleep(logInterval * 60);
      scheduledWakeup = 1;
    }
    calculateRainGaugeData();
  }

}
void initSyncTime()
{
  initConnection();
  Particle.syncTime();
}
void initConnection()
{
  Cellular.on();
  Cellular.connect();
  waitUntil(Cellular.ready);
  Particle.connect();
  waitUntil(Particle.connected);
}
void disconnectConnection()
{
  Particle.disconnect();
  waitUntil(Particle.disconnected);
  Cellular.off();
}
void calculateRainGaugeData()
{
  if (buttonState == HIGH)
  {
  // turn LED on:
  //Serial.println("HIGH");
  }
  else
  {
    tripCount++;
    delay(400);
    Serial.println(tripCount);
  }
}