#include <WiFi.h>
#include <Preferences.h>
#include <MQTTClient.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <dht.h>



#define DHTTYPE DHT22 



Preferences preferences;
WiFiClient net;
PubSubClient MQTTclient(net);


// TO BE MODIFIED WHEN CREATING A NEW DEVICE ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const char*  devId = "DEV-709"; 
String devId_str= "DEV-709";

char* Sub_reg_char = "DEV-709/REG";

const char* PIN_printed ="1234";
String PIN_printed_str = "1234";



// On Original dev circuit.

//const int ledPin = 12;
//const int ledPin_button = 14;
//const int buttonPin = 26;
//const int ledPin_water = 25;
//#define DHTPIN 15

#define DHTPIN 15
const int ledPin = 5;
const int ledPin_button = 18;
const int buttonPin = 23;
const int ledPin_water = 19;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

DHT dht(DHTPIN, DHTTYPE);
unsigned long time_pressed_buttonPin=0;
unsigned long timeBefore_pressed_buttonPin=0;

// [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] 
const char* ssid     ="TIM-95582885";//   "Telecom-47442901"" superschool" "supercampus""peace"
const char* password ="onejfu7Rpyo5B2rXNH5hX5ck";//; "baiguerabaiguerabaiguera""D1g1t@l!" ""superfaggin""lucieombre"
// metti ssid e password  
// [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] [%] 

unsigned long lastMillis = 0;
unsigned long lastMillisReg = 0;

const char* mqtt_server = "159.89.10.11";
const char* mqttUser= "astroteo";
const char* mqttPassword= "Gold4Himself";

long lastMsg = 0;
char msg[20];


float localHum = 0;
float localTemp = 0;


String ACTpayload="none";
int ACTstate = 0;

String REGpayload = "none";

bool notConnectedFirst = true;

const char*  new_dev= "NEW-DEV";

String devId_and_Pin = devId_str+'/'+PIN_printed_str;

char* token= "none";
String token_str = "none";
String MQTTreg = "NO";


char* guess_char="?";
char* OK_char = "OK";



int time_to_connect =0;
bool restart_flag = true;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void receivedREGCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received in : ");
  Serial.print(topic);
  Serial.print(", payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();


if (String(topic).equals(devId_str+"/REG")){
  
   REGpayload = String((char*)payload);
 
    }

    else{

     REGpayload = "none";
      
      }
 }


void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received in : ");
  Serial.print(topic);
  Serial.print(", payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();


if (String(topic).equals(token_str+"/ACT")){
  
ACTpayload = String((char)payload[0])+String((char)payload[1]);
}
else{
  ACTpayload ="none";
 }
  
  }
void mqttconnectFirst(){
      String Sub_reg= devId_str+"/REG";
      char Sub_reg_char[Sub_reg.length()+1];
      Sub_reg.toCharArray(Sub_reg_char,Sub_reg.length()+1);
      
/* Loop until reconnected */
while (!MQTTclient.connected()) {
    Serial.print("MQTT connecting ...");
    
    
    /* connect now */
    if (MQTTclient.connect(devId,mqttUser,mqttPassword)) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      MQTTclient.subscribe(new_dev);
      MQTTclient.subscribe(Sub_reg_char);
      
      
    } else {
      Serial.print("failed, status code =");
      Serial.print(MQTTclient.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
  
}


void mqttconnect() {
  /* Loop until reconnected */
  while (!MQTTclient.connected()) {
    Serial.print("MQTT connecting ...");
    
   
    /* connect now */
    if (MQTTclient.connect(devId,mqttUser,mqttPassword)) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/

      String Sub_T= token_str+"/T";
      char Sub_T_char[Sub_T.length()+1];
      Sub_T.toCharArray(Sub_T_char,Sub_T.length()+1);
      
      MQTTclient.subscribe(Sub_T_char);


      String Sub_H= token_str+"/H";
      char Sub_H_char[Sub_H.length()+1];
      Sub_H.toCharArray(Sub_H_char,Sub_H.length()+1);
      
      MQTTclient.subscribe(Sub_H_char);


      String Sub_ACT= token_str+"/ACT";
      char Sub_ACT_char[Sub_ACT.length()+1];
      Sub_ACT.toCharArray(Sub_ACT_char,Sub_ACT.length()+1);
      
      MQTTclient.subscribe(Sub_ACT_char);
      
      
    } else {
      Serial.print("failed, status code =");
      Serial.print(MQTTclient.state());
      Serial.println(", try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}


void getDHT() {
    float tempIni = localTemp;
    float humIni = localHum;
    localTemp = dht.readTemperature();
    localHum = dht.readHumidity();
    if (isnan(localHum) || isnan(localTemp))   // Check if any reads failed and exit early (to try again).
    {
      localTemp = tempIni;
      localHum = humIni;
      return;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  Serial.begin(115200);
   
  preferences.begin("MQTT_pref",false);
  MQTTreg = preferences.getString("MQTT_reg","NO");
  token_str = preferences.getString("token","none");
  preferences.end();
  
  WiFi.begin(ssid, password);
  delay(10000);
  // put your setup code here, to run once:
  pinMode (ledPin_button, OUTPUT);
  pinMode(ledPin_water,OUTPUT);
  pinMode(buttonPin,INPUT);
  pinMode(ledPin,OUTPUT);


  
  Serial.println("MQTT registered ??"+MQTTreg);
  Serial.println("stored token ??"+token_str);
  

    if (MQTTreg.equals("YES")){
      notConnectedFirst = false;
    }

    while (WiFi.status() != WL_CONNECTED && millis()- time_to_connect < 300000 ) {
                //Wifi Connect
                //Serial.println("Connecting to WiFi..");

                if (WiFi.status() == WL_CONNECTED){
                     restart_flag = false;
                     }
                }
             
    if(restart_flag){


ESP.restart();
      
      }
else{
    Serial.println("Connected to the WiFi network");
           
              
   Serial.println("IP-Address: "); 
   Serial.println(WiFi.localIP()); 
   delay(1000);

}

   MQTTclient.setServer(mqtt_server, 8883);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */

  if (notConnectedFirst)
  {

   MQTTclient.setCallback(receivedREGCallback);

  }
  else{
   MQTTclient.setCallback(receivedCallback);
    }
    
  }

void loop(){
  
timeBefore_pressed_buttonPin =0;

if(digitalRead(buttonPin) == HIGH )
{ 
  time_pressed_buttonPin = millis();
 
   
   Serial.println("tiempo before"+String(timeBefore_pressed_buttonPin));
  //Serial.println("tiempo "+String(time_pressed_buttonPin));
  digitalWrite(ledPin_button,HIGH);//turn on the led


  if ( (time_pressed_buttonPin-timeBefore_pressed_buttonPin) >= 5000){
    
    Serial.println("t - t_before"+String(time_pressed_buttonPin-timeBefore_pressed_buttonPin));
    Serial.print("resetting....");
/*
  preferences.begin("wifi",false);
  preferences.putString("ssid","none");
  Serial.println(preferences.getString("ssid","none"));
  preferences.getString("pswd","none");
  Serial.println(preferences.getString("pswd","none"));
  preferences.end();
  */


  preferences.begin("MQTT_pref",false);
  preferences.putString("MQTT_reg","NO");
  preferences.putString("token","none");
  preferences.end();

  ESP.restart();
  
  }
  
}
else
{
 digitalWrite(ledPin_button,LOW);//turn off the led
 timeBefore_pressed_buttonPin =millis();

//Fire led if Wifi is connected to Wifi
  if (WiFi.status() == WL_CONNECTED){
    digitalWrite(ledPin, HIGH);   
    }


  if (!MQTTclient.connected() && notConnectedFirst) {
    Serial.println("Here: subscribing to NEW-DEV");
    mqttconnectFirst();
    
    
  }
  
  else if(!MQTTclient.connected() && notConnectedFirst==false) {
    //Serial.println("??HERE??");
    mqttconnect();
  }
  
  getDHT();
  MQTTclient.loop();

  if(notConnectedFirst){

   if (millis()-lastMillisReg > 2000){
      Serial.println("Here: publishing on NEW-DEV topic");
      lastMillisReg = millis();
      
      char devId_and_Pin_char[devId_and_Pin.length()+1];
      devId_and_Pin.toCharArray(devId_and_Pin_char,devId_and_Pin.length()+1);
      
      MQTTclient.publish(new_dev,devId_and_Pin_char);
      Serial.println("Sub_reg_char??");
      Serial.println(Sub_reg_char);
      MQTTclient.publish(Sub_reg_char,guess_char);
      }
      


    //Serial.println("The  REGpayload is: "+REGpayload);
    //Serial.println("The  REGpayload length is: "+String(REGpayload.length()));
    

   if(REGpayload.length() > 63){

      Serial.println(" OK !!! token received !!!");
      notConnectedFirst = false;

      preferences.begin("MQTT_pref",false);
      preferences.putString("MQTT_reg","YES");
      preferences.putString("token",REGpayload);
      preferences.end();

      
      MQTTclient.publish(Sub_reg_char,OK_char);
      
      ESP.restart();
      
      
      }


    
    }

    
   else{


    


    String Sub_ACT = token_str+"/ACT";
    char Sub_ACT_char[Sub_ACT.length()+1];
    Sub_ACT.toCharArray(Sub_ACT_char,Sub_ACT.length()+1);
  

   String startDrop = "start dropping water";
   char startDrop_char[startDrop.length()+1];
        startDrop.toCharArray(startDrop_char,startDrop.length()+1);


   String stopDrop = "stop dropping water";
   char stopDrop_char[stopDrop.length()+1];
        stopDrop.toCharArray(stopDrop_char,stopDrop.length()+1);
        

   if ((char)ACTpayload[0]=='1'){

    ACTstate = 1 ;
    
    //MQTTclient.publish(Sub_ACT_char,startDrop_char);
   
    }
    else if((char)ACTpayload[0]=='0'){

   ACTstate = 0;

   //MQTTclient.publish(Sub_ACT_char,stopDrop_char);
     
      }


    if (ACTstate ==1){
      
      digitalWrite(ledPin_water,HIGH);
      
      }
      else if (ACTstate==0){
        
      digitalWrite(ledPin_water,LOW);
      
        }

//  [*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*]
if (millis() - lastMillis > 2000) {
//2000::::: aumentare -> pubblica di meno....  diminuire-> pubblica di più
//  [*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*][*]
    
    String local_temp=String(localTemp);
    String local_hum=String(localHum);
    
    char local_temp_char[local_temp.length()+1];
     local_temp.toCharArray(local_temp_char,local_temp.length()+1);

    char local_hum_char[local_hum.length()+1];
     local_hum.toCharArray(local_hum_char,local_hum.length()+1);
    
    lastMillis = millis();
    //client.publish("DEV-1/ACT", "dropping-water");
    
    String Sub_T = token_str+"/T";
      char Sub_T_char[Sub_T.length()+1];
      Sub_T.toCharArray(Sub_T_char,Sub_T.length()+1);
      MQTTclient.publish(Sub_T_char, local_temp_char);
    
   String Sub_H = token_str+"/H";
      char Sub_H_char[Sub_H.length()+1];
      Sub_H.toCharArray(Sub_H_char,Sub_H.length()+1);
      MQTTclient.publish(Sub_H_char, local_hum_char);
   
    }
   }
  

}
}

  
    
    


  


  

  

 
