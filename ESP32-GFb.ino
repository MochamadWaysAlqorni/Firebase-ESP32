#include "max6675.h"
#include <WiFi.h>
#include <FirebaseESP32.h>
#define FIREBASE_HOST "INSERT_HOST"
#define FIREBASE_AUTH "INSERT_SECRET"
FirebaseData firebaseData;
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read(); 
int thermoDO = 19;
int thermoCS = 23;
int thermoCLK = 5;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
#define LED 2
int jenisikan=0;
float suhucelcius=0;
int reporterstop=0;
int heater=0;
unsigned long waktusekarang=0;
TaskHandle_t Task1;
TaskHandle_t Task2;

void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  WiFi.disconnect();
  delay(1000);
  WiFi.begin("SSID","PASWOT");
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
  }
  Serial.println((WiFi.localIP()));
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setInt(firebaseData, "/DATA/ikan", (0));
  Firebase.setInt(firebaseData, "/DATA/stop", (0));


xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    2,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 
}


//Task1code: 
void Task1code( void * pvParameters ){
  for(;;){
    kirimFb();
    delay(3000);
  } 
}

//Task2code: 
void Task2code( void * pvParameters ){
  for(;;){
    pewaktu();
  }
}

void loop() {

}

void kirimFb(){
  int loopwhl=1;
  while(loopwhl>=0){
    Firebase.getInt(firebaseData, "/DATA/stop", (reporterstop));
    Serial.print(reporterstop);
    Serial.print(",");
    if (reporterstop==1){
      Firebase.setInt(firebaseData, "/DATA/stop", (0));
      Firebase.setInt(firebaseData, "/DATA/ikan", (0));
      ESP.restart();
    }
    else {
      break;
    }
  }
  suhucelcius=(thermocouple.readCelsius());
  Serial.print(suhucelcius);
  Serial.print(",");
  if ((Firebase.setFloat(firebaseData, "/DATA/temp", (suhucelcius))) == true) {
      Serial.print("sent-ok");
    } else {
      Serial.print("sent-er");
    }

  heater=digitalRead(2);
  if (heater==1){
    Firebase.setInt(firebaseData, "/DATA/heater", (1));
  }
  else{
    Firebase.setInt(firebaseData, "/DATA/heater", (0));
  }
  Serial.print(",");
  Serial.println(heater);
}

void pewaktu(){
  waktusekarang=millis();
  Firebase.getInt(firebaseData, "/DATA/ikan", (jenisikan));
  delay(1000);
  if (jenisikan==1){ //lele5400000(90menit)
        suhucelcius=(thermocouple.readCelsius());
        while(millis()<waktusekarang+5400000){
        if(suhucelcius>100){
          digitalWrite(2,LOW);}
        else{
          digitalWrite(2,HIGH);}        
  }
  ESP.restart();
  }
  if (jenisikan==2){ //patin7200000(120menit)
        //Firebase.setInt(firebaseData, "/DATA/status", (1));
        suhucelcius=(thermocouple.readCelsius());
        while(millis()<waktusekarang+7200000){
        if(suhucelcius>100){
          digitalWrite(2,LOW);}
        else{
          digitalWrite(2,HIGH);}
  }
  ESP.restart();
  }
  if (jenisikan==3){ //nila3600000(60menit)
        suhucelcius=(thermocouple.readCelsius());
        while(millis()<waktusekarang+3600000){
        if(suhucelcius>100){
          digitalWrite(2,LOW);}
        else{
          digitalWrite(2,HIGH);}
  }
  ESP.restart();
  }
  
  else{
    digitalWrite(2,LOW);
  }
}
