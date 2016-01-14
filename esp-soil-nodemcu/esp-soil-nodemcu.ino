#include <ESP8266WiFi.h>
#include <OneWire.h>

#define myPeriodic 15
//OneWire oneWire(ONE_WIRE_BUS);

const int pin_clk = 4;
const int pin_soil = A0;
const int pin_led = 5;
float prevTemp = 0;
const char* server = "api.thingspeak.com";
String apiKey ="3QJUQ8I818FYEXQ9";
const char* MY_SSID = "siyue1L"; 
const char* MY_PWD = "siyuexiongdi";
int sent = 0;
#define SLEEPTIME 5*60*1000*1000

void setup() {
  Serial.begin(115200);
  pinMode(pin_clk, OUTPUT);
  pinMode(pin_soil, INPUT);
  pinMode(pin_led, OUTPUT);
  digitalWrite(pin_led, LOW);
  analogWriteFreq(40000);
  analogWrite(pin_clk, 400);
  connectWifi();
}

void loop() {
  float temp;
  //char buffer[10];
  Serial.println("Requesting humidity...");
  temp = Get_Val(8);
  //String tempC = dtostrf(temp, 4, 1, buffer);//handled in sendTemp()
  Serial.print(String(sent)+" humidity: ");
  Serial.println(temp);

  digitalWrite(pin_led, HIGH);
  delay(500);
  digitalWrite(pin_led, LOW);
  delay(500);
  //if (temp != prevTemp)
  //{
  //sendTeperatureTS(temp);
  //prevTemp = temp;
  //}
  
  sendTeperatureTS(temp);
//  int count = myPeriodic;
//  while(count--)
//  delay(1000);
  Serial.println("The System will sleep!");
  ESP.deepSleep(SLEEPTIME,WAKE_RF_DEFAULT);
}

float Get_Val(int n)
{
  float valtemp = 0;
  float ValBuf[n];
  int i = 0;
  int j = 0;
  for(i=0;i<n;i++){
    ValBuf[i] = analogRead(pin_soil);  
  }

  for(i=0;i<n-1;i++){
    for(j=i+1;j<n;j++){
      if(ValBuf[i]>ValBuf[j]){
        valtemp = ValBuf[i];
        ValBuf[i] = ValBuf[j];
        ValBuf[j] = valtemp;
      }
    }
  }
  valtemp = 0;
  for(i=1;i<n-1;i++){
    valtemp+=ValBuf[i];
  }
  valtemp/=(n-2);
  return valtemp;
}

void connectWifi()
{
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
}//end connect

void sendTeperatureTS(float temp)
{  
   WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(temp);
   postStr += "\r\n\r\n";
   
   client.print("POST /update HTTP/1.1\n");
   Serial.println("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   Serial.println("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   Serial.println("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   Serial.println("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   Serial.println("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   Serial.println("Content-Length: ");
   client.print(postStr.length());
   Serial.println(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   Serial.println(postStr);
   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}//end send
