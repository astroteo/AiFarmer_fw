#include <WiFi.h>
#include <Preferences.h>
#include <MQTTClient.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <dht.h>

#define DHTPIN 27 
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);

Preferences preferences;
WiFiClient net;
PubSubClient client(net);


int ledPin = 12;
const int ledPin_button = 14;
const int buttonPin = 26;
const int ledPin_water = 25;
unsigned long time_pressed_buttonPin=0;
unsigned long timeBefore_pressed_buttonPin=0;

const char* ssid     ="Telecom-47442901" ;// "supercampus" "superschool"
const char* password = "baiguerabaiguerabaiguera";//  ;"D1g1t@l!"
unsigned long lastMillis = 0;


const char* mqtt_server = "159.89.10.11";
const char* mqttUser= "astroteo";
const char* mqttPassword= "Gold4Himself";


float localHum = 0;
float localTemp = 0;



long lastMsg = 0;
char msg[20];

String ACTpayload="0";
String REGpayload = "NO";

bool not_registered_device = true;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received in : ");
  Serial.print(topic);

  if (String(topic).equals("DEV-1/ACT")){
  ACTpayload = String((char*)payload);
}
else{
  ACTpayload ="None";
  }

  if (String(topic).equals("NEW-DEV")){
  REGpayload = String((char*)payload);
}
else{
  REGpayload ="NO";
  }

  Serial.print(", payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  /* we got '1' -> on */
  if ((char)payload[0] == '1') {
   digitalWrite(ledPin_water, HIGH);
  } else {
    /* we got '0' -> on */
    digitalWrite(ledPin_water, LOW);
  }

}

void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "DEV-1";
    /* connect now */
    if (client.connect("DEV-1",mqttUser,mqttPassword)) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      client.subscribe("NEW-DEV");
      client.subscribe("DEV-1/ACT");
      client.subscribe("DEV-1/T");
      client.subscribe("DEV-1/H");
      
      
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 WiFi.begin(ssid, password);
 delay(10000);
  while (WiFi.status() != WL_CONNECTED) {
                //Wifi Connect
                Serial.println("Connecting to WiFi..");
                }
             
              Serial.println("Connected to the WiFi network");
           
              
   Serial.println("IP-Address: "); 
   Serial.println(WiFi.localIP()); 
   delay(1000);


  client.setServer(mqtt_server, 8883);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */
  client.setCallback(receivedCallback);

}

void loop() {
  // put your main code here, to run repeatedly:

  if (!client.connected()) {
    mqttconnect();
  }

  
  
 client.loop();
int now = millis();
int nowR = millis();

while(not_registered_device){
  

    if( millis()-now > 1000){
     
    now = millis();
    client.publish("NEW-DEV","DEV-1");
    }

       if( millis()-nowR > 10000){
     
    Serial.println(REGpayload);
    }
    
    if(REGpayload.equals("OK")){
    client.unsubscribe("NEW-DEV");
    Serial.println("unsubscribed from registration channel");
    not_registered_device = false;
    
    }
    
   
}
}
