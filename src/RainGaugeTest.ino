


int led1 = D0; // Instead of writing D0 over and over again, we'll write led1
// You'll need to wire an LED to this one to see it blink.

int led2 = D7; // Instead of writing D7 over and over again, we'll write led2
// This one is the little blue LED on your board. On the Photon it is next to D7, and on the Core it is next to the USB jack.

const int buttonPin = D4;
unsigned long prevMinute;
unsigned long sendInterval = 60; // send intereval in minutes //RT change if necessary
unsigned long logInterval = 5; // Set the log interval in minutes //RT change if necessary
unsigned long currentMinute;
int counter = 0;
int startupCounter = 0;
int i = 0;
unsigned long temp = 0;
int buttonState = 0;
char payload[128];


SYSTEM_MODE (MANUAL);


void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  prevMinute = Time.minute();
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
}
void loop()
{
  //System.wakeup();
  currentMinute = Time.minute();
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  if ((currentMinute - prevMinute >= logInterval) && counter)
  {
       //log the data
    if (temp == 0)
    {
    strcat(payload,String(i));
    }
    else 
    {
    strcat(payload,",");
    strcat(payload,String(i));
    }

    Serial.print("Payload:");
    Serial.println(payload);
    Serial.println(prevMinute);
     
    temp++;
    i = 0;
    prevMinute = currentMinute;

    if (temp >= sendInterval/logInterval)
    {
    //initiliaze connection, Cellular and Particle cloud handshake
    initConnection();
    
    //send Message
    Particle.publish("StatusRainGauge",payload ,PRIVATE);
    //Particle.publish("Temp", "{ 'id':'1C93F9','data':'ffffffffffffffff30','time':'1570476362','device':'1C93F9','lqi':'Good'}", PRIVATE);
    Serial.println("Message sent");
    strcpy(payload,"");
    //prevMinute = Time.minute();
    temp = 0;
    counter = 0;
    }    
  }
  else
  {

    if (!counter)
    {
    //Turn of everything
    disconnectConnection();
    //Go to sleep
    Serial.println("Sleep");
    Serial.println("");
    System.sleep(logInterval * 60);
    counter = 1;
    }

    //Get the rain gauge data 
    calculateRainGaugeData();
  }

    //Serial.println("Sleep");
    //publish("warning", "Call setEnabled() to enable deep sleep");
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
  i++;
  delay(400);
  Serial.println(i);
  }
}