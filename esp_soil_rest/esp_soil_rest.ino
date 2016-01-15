

//const int pin_clk = 4;
#define pin_led  4

#define PIN_ENABLE_ESP 13
#define SLEEP_TIME 5*1000*1000
 int FIGURE = 1;

void setup() {
//  pinMode(pin_clk, OUTPUT);
//  pinMode(pin_soil, INPUT);
  Serial.begin(9600);
  pinMode(pin_led,OUTPUT);
  digitalWrite(pin_led,LOW);

  delay(500);
 
//  analogWriteFreq(30000);
//  analogWrite(pin_clk, 4000);
}
  
void loop() {
 // val = analogRead(pin_soil);
 // Serial.println(val);

  if(FIGURE == 1){
    int i = 0;
     for( i=0;i<10;i++){
        digitalWrite(pin_led,HIGH);
        delay(500);
        digitalWrite(pin_led,LOW);
        delay(500);
      }
    FIGURE = 0;
   }else{    
      ESP.deepSleep(SLEEP_TIME,WAKE_RF_DEFAULT);
      FIGURE = 1;
   }
}
