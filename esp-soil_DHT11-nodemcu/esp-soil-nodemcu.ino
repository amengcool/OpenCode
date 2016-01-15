#include <ESP8266WiFi.h>
#include <dht11.h>

dht11 DHT;

const int pin_clk = 4;
const int pin_soil = A0;
const int pin_led = 5;
float prevTemp = 0;
const int pin_read = 2;

const char* host = "api.thingspeak.com";
String apiKey ="YOUR APIKEYS";
const char* MY_SSID = "YOUR SSID"; 
const char* MY_PWD = "YOUR PASSWORD";

#define SLEEPTIME 5*60*1000*1000

void setup() {
  Serial.begin(115200);
  Pin_Init();
  delay(500);
  connectWifi();
}

void loop() {
  float soil_hum,dht_hum,dht_temp;
  //char buffer[10];
  Serial.println("Requesting soil_humidity,dht_humidity,dhi_temperature...");
  soil_hum = Get_Soil_Val(8);
  Serial.print("soil_humidity: ");
  Serial.println(soil_hum);
 
 //read DHT1 datas
  int chk = DHT.read(pin_read);
  if(chk == DHTLIB_OK){
  //  char buff[64];
    char str_hum[6],str_temp[6];
    dht_hum = DHT.humidity;
    dht_temp = DHT.temperature;
  }
  Serial.print("dht_humidity: ");
  Serial.println(dht_hum);
  Serial.print("dht_temperature: ");
  Serial.println(dht_temp);
  delay(1000);
  //let the led sight 5s
  digitalWrite(pin_led, HIGH);
  delay(5000);
  digitalWrite(pin_led, LOW);
  delay(5000);

  //send datas to server
  sendTeperatureTS(soil_hum,dht_hum,dht_temp);
  Serial.println("");
  Serial.println("The System will sleep!\r\n");
  //ESP8266 deepsleep 5mins
  ESP.deepSleep(SLEEPTIME,WAKE_RF_DEFAULT);
}

void Pin_Init(void)
{
  pinMode(pin_clk, OUTPUT);
  pinMode(pin_soil, INPUT);
  pinMode(pin_led, OUTPUT);
  digitalWrite(pin_led, LOW);
//  pinMode(pin_vcc, OUTPUT);
  pinMode(pin_read, INPUT);
//  digitalWrite(pin_vcc, HIGH);
  
  analogWriteFreq(40000);
  analogWrite(pin_clk, 400);
}
//read soil humidity data
//读取n次 排序 取中间n-2次的平均值
float Get_Soil_Val(int n)
{
  float valtemp = 0;
  float ValBuf[n];
  int i = 0;
  int j = 0;
  //读取n次
  for(i=0;i<n;i++){
    ValBuf[i] = analogRead(pin_soil);  
  }
 //排序
  for(i=0;i<n-1;i++){
    for(j=i+1;j<n;j++){
      if(ValBuf[i]>ValBuf[j]){
        valtemp = ValBuf[i];
        ValBuf[i] = ValBuf[j];
        ValBuf[j] = valtemp;
      }
    }
  }

  //取中间的n-2次的平均值
  valtemp = 0;
  for(i=1;i<n-1;i++){
    valtemp+=ValBuf[i];
  }
  valtemp/=(n-2);
  return valtemp;
}
//connect to your AP
void connectWifi()
{
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
}//end connect

//send datas to server
void sendTeperatureTS(float soil_hum,float dht_hum,float dht_temp)
{  
  Serial.print("Connecting to ");
  Serial.println(host);
  WiFiClient client;
  
   if (client.connect(host, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
     Serial.println("WiFi Client connected ");
  
  // We now create a URI for the request  
     String postStr = apiKey;
     postStr += "&field1=";
     postStr += String(soil_hum);
     postStr += "&field2=";
     postStr += String(dht_hum);
     postStr += "&field3=";
     postStr += String(dht_temp);
     postStr += "\r\n\r\n";
  
     Serial.print("Requesting URL:");
     Serial.println(postStr);
/*
    client.print(String("POST") + postStr +  "/update HTTP/1.1\n" +
                 "HOST: " + host + "\n" +
                 "Connection: close\r\n\r\n");
*/  
  // This will send the request to the server              
   client.print("POST /update HTTP/1.1\n");
   client.print("Host: api.thingspeak.com\n");
   client.print("Connection: close\n");
   client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
   client.print("Content-Type: application/x-www-form-urlencoded\n");
   client.print("Content-Length: ");
   client.print(postStr.length());
   client.print("\n");
   client.print(postStr);
   delay(1000);
   
     Serial.println("Respond:");
     while(client.available()){
       String line = client.readStringUntil('\r');
       Serial.print(line);
     }
  }//end if
  client.stop();
}//end send
