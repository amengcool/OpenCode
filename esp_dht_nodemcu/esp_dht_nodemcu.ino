#include <ESP8266WiFi.h>
#include <dht11.h>

#define myPeriodic 5

dht11 DHT;
const int pin_vcc = 4;
const int pin_read = 5;
float prevTemp = 0;
const char* server = "api.thingspeak.com";
String apiKey ="YOUR API_KEYS";
const char* MY_SSID = "YOUR SSID"; 
const char* MY_PWD = "YOUR PASSWORD";
int sent = 0;

void setup() {
  Serial.begin(115200);
  pinMode(pin_vcc, OUTPUT);
  pinMode(pin_read, INPUT);
  digitalWrite(pin_vcc, HIGH);
  delay(500);
  connectWifi();
}

void loop() {
  //char buffer[10];
  float hum,temp;
  Serial.println("Requesting Temperature and Humidity...");
    int chk = DHT.read(pin_read);
    if(chk == DHTLIB_OK){
      char buff[64];
      char str_hum[6],str_temp[6];
      hum = DHT.humidity;
      temp = DHT.temperature;
      dtostrf(hum, 4, 2, str_hum);
      dtostrf(temp, 4, 2, str_temp);
      sprintf(buff,"api_key=NA14SN41BLPVFG5E&field1=%s&field2=%s",str_hum,str_temp);
        Serial.println(buff);

    }
  
  sendTeperatureTS(hum,temp);
  int count = myPeriodic;
  while(count--)
  delay(1000);
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

void sendTeperatureTS(float hum, float temp)
{  
   WiFiClient client;
  
   if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
   Serial.println("WiFi Client connected ");
   
   String postStr = apiKey;
   postStr += "&field1=";
   postStr += String(hum);
   postStr += "&field2=";
   postStr += String(temp);
   postStr += "\r\n\r\n";
   
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n\n");
   client.print(postStr);
   delay(1000);
   
   }//end if
   sent++;
 client.stop();
}//end send
