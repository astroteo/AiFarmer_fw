#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoOTA.h>
#include <dummy.h>
#include <dht.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <Preferences.h>
#include <MQTTClient.h>
#include <PubSubClient.h>
#define DHTTYPE DHT22 
// TO BE MODIFIED WHEN CREATING A NEW DEVICE ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const char*  devId = "DEV-10002"; 
String devId_str= "DEV-10002";
char* Sub_reg_char = "DEV-10002/REG";//  !!! ../REG --> deve essere uguale a devId 
const char* PIN_printed ="1234";
String PIN_printed_str = "1234";
char* devId_and_Pin_char="DEV-10002/1234";
//AP-mode
const char* AP_SSID = "AI-FARMER_DEV-10002";


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

  
//DHT dht(DHTPIN, DHTTYPE);
  float localHum = 0;
  float localTemp = 0;

  float localTemp_last_transmit =0;

// Connection-type & parameters
   
   //Wifi-connection
   bool wifi_connected = false;
   bool WifiConnectedDevice = false;
   

  char*qssid = "";
  char*qpswd = " ";
  
  const char *AP_PSWD = "12345678";
  //AP-mode --> Wifi-mode
  static volatile bool got_ssid_from_AP = false;
  static volatile bool got_pswd_from_AP = false;
  // wifi-test 
  unsigned long start_WiFi_test;
  // APP communication.
  int comm_app = 0;
  

  // set server fo AP mode.
WiFiServer server(80);
Preferences preferences;
WiFiClient net;
//PubSubClient MQTTclient(net);
MQTTClient MQTTclient;

DHT dht(DHTPIN, DHTTYPE);
unsigned long time_pressed_buttonPin=0;
unsigned long timeBefore_pressed_buttonPin=0;

const char* ssid     = "supercampus" ;//   "Telecom-47442901"" superschool""peace"
const char* password = "superfaggin";//  ; "baiguerabaiguerabaiguera""D1g1t@l!""lucieombre" 
unsigned long lastMillis = 0;
unsigned long lastMillisReg = 0;

const char* mqtt_server = "159.89.10.11";
const char* mqttUser= "astroteo";
const char* mqttPassword= "Gold4Himself";

long lastMsg = 0;
char msg[20];



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

// Button-Issue
bool button_issue = false;

int time_to_connect =0;
bool WiFi_test=false;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void receivedREGCallback(String &topic, String &payload) {
  Serial.print("Message received in : ");
  Serial.print(topic);
  Serial.print(", payload: ");
  
    Serial.print(payload);
  
  Serial.println();


if (String(topic).equals(devId_str+"/REG")){
  
   REGpayload =payload;
 
    }

    else{

     REGpayload = "none";
      
      }
 }


void receivedCallback(String &topic, String &payload) {
  Serial.print("Message received in : ");
  Serial.print(topic);
  Serial.print(", payload: ");
Serial.print(payload);
  
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
      
      } 
    else {
      Serial.print("failed, status code =");
      
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
  MQTTclient.subscribe(new_dev);
  MQTTclient.subscribe(Sub_reg_char);
}


void mqttconnect() {


  String Sub_T= token_str+"/T";
      char Sub_T_char[Sub_T.length()+1];
      Sub_T.toCharArray(Sub_T_char,Sub_T.length()+1);


   String Sub_H= token_str+"/H";
      char Sub_H_char[Sub_H.length()+1];
      Sub_H.toCharArray(Sub_H_char,Sub_H.length()+1);


    String Sub_ACT= token_str+"/ACT";
      char Sub_ACT_char[Sub_ACT.length()+1];
      Sub_ACT.toCharArray(Sub_ACT_char,Sub_ACT.length()+1);

  
  /* Loop until reconnected */
  while (!MQTTclient.connected()) {
    Serial.print("MQTT connecting ...");
    
   
    /* connect now */
    if (MQTTclient.connect(devId,mqttUser,mqttPassword)) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/

      
      
      
      
    }
  }

  
      MQTTclient.subscribe(Sub_T_char);
      MQTTclient.subscribe(Sub_H_char);
      MQTTclient.subscribe(Sub_ACT_char);
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
delay(5000);// DEBUG: time to open serial for check preferences

// pinMode setup
// put your setup code here, to run once:
pinMode (ledPin_button, OUTPUT);
pinMode(ledPin_water,OUTPUT);
pinMode(buttonPin,INPUT);
pinMode(ledPin,OUTPUT);
digitalWrite(ledPin, LOW);


//******************************************************************************
// begin wifi preferences --> acces r/w permanent memory of ESP32
  preferences.begin("wifi_pref",false);
  Serial.println("...get_preferences");
  String ssid_st = preferences.getString("ssid_from_AP","none");
  String pswd_st = preferences.getString("pswd_from_AP","none");
  preferences.end();



Serial.println("ssid from AP:"+ssid_st);
char ssid_st_char[ssid_st.length()+1];
     ssid_st.toCharArray(ssid_st_char,ssid_st.length()+1);


Serial.println("pswd from AP:"+pswd_st);
char pswd_st_char[pswd_st.length()+1];
     pswd_st.toCharArray(pswd_st_char,pswd_st.length()+1);
     
bool wifi_connected_once = false;

//*********************************************************************************

if (ssid_st.equals("none") ||  pswd_st.equals("none") )
{ }
else
{ wifi_connected_once = true;}


Serial.println("This device has already been connected to the internet ??");
Serial.println(String(wifi_connected_once));



// !! replacED: WiFiDirectConnect --> wifi_connected_once, remove: TrueTest
if (wifi_connected_once)
{

  Serial.println(" ???ENTERED HERE--> connecting to WiFi???");
  WiFi.begin(ssid_st_char,pswd_st_char);
  while (WiFi.status() != WL_CONNECTED && millis()-time_to_connect < 30000) {
                //Wifi Connect
                //Serial.println("Connecting to WiFi..");
                }

              if(WiFi.status() == WL_CONNECTED){ 
             
              Serial.println("Connected to the WiFi network");
              WifiConnectedDevice = true;

              }
              else{
                Serial.println("restarting for tryiing connect again");
                delay(1000);
                ESP.restart();
                }
              
   Serial.println("IP-Address: "); 
   Serial.println(WiFi.localIP()); 
   delay(1000);

   wifi_connected = true;
   
    

  preferences.begin("MQTT_pref",false);
  MQTTreg = preferences.getString("MQTT_reg","NO");
  token_str = preferences.getString("token","none");
  preferences.end();
  
  Serial.println("MQTT registered ??"+MQTTreg);
  Serial.println("stored token ??"+token_str);
  

  if (MQTTreg.equals("YES")){
      notConnectedFirst = false;
    }

   MQTTclient.begin("mqtt.aifarmer.du.cdr.mn",8883,net);
  
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */

  if (notConnectedFirst)
  {

   //MQTTclient.setCallback(receivedREGCallback);
    MQTTclient.onMessage(receivedREGCallback);
  }
  else{
   MQTTclient.onMessage(receivedCallback);
    }
}
 
else {

  Serial.println("!!! ENTERED HERE !!!");
  Serial.print("Configuring access point...\n");
  WiFi.mode(WIFI_MODE_APSTA);


 WiFi.softAPsetHostname(AP_SSID);
 WiFi.softAPenableIpV6();//enable ap ipv6 here
 WiFi.softAP(AP_SSID);
 
 
 IPAddress myIP = WiFi.softAPIP();
 Serial.print("AP --> IP-Address: ");
 Serial.println(myIP);
 delay(10000);
 server.begin();

wifi_connected = false;
  }
  
  

}

void loop() {

if (wifi_connected) {
  
    wifiConnectedLoop();
    
  } else {
    wifiDisconnectedLoop();
  }

}

void wifiConnectedLoop() {

if (WiFi.status() == WL_CONNECTED){
  digitalWrite(ledPin, HIGH);   
  }
  else{
  digitalWrite(ledPin, LOW);
    }

  if(digitalRead(buttonPin) == HIGH ){ 
  time_pressed_buttonPin = millis();
 
 
   //Serial.println("tiempo before"+String(timeBefore_pressed_buttonPin));
  //Serial.println("tiempo "+String(time_pressed_buttonPin));
  digitalWrite(ledPin_button,HIGH);//turn on the led

  if ( (time_pressed_buttonPin-timeBefore_pressed_buttonPin) >= 5000){
    
    Serial.println("t - t_before"+String(time_pressed_buttonPin-timeBefore_pressed_buttonPin));
    Serial.print("resetting Wifi preferences...");



  
   button_issue= true;


   if(button_issue){

  preferences.begin("wifi_pref",false);
  String SSID_sst = preferences.getString("ssid_from_AP","none");
  
  Serial.println(SSID_sst);
  String PSWD_sst =preferences.getString("pswd_from_AP","none");
  Serial.println(PSWD_sst);
  preferences.end();

  
  preferences.begin("wifi_pref",false);
  preferences.putString("ssid_from_AP","none");
  //Serial.println(preferences.getString("ssid","none"));
  preferences.putString("pswd_from_AP","none");
  //Serial.println(preferences.getString("pswd","none"));
  preferences.end();


  preferences.begin("wifi_pref",false);
  String SSID_sst_AFTER = preferences.getString("ssid_from_AP","none");
  Serial.println(SSID_sst_AFTER);
  String PSWD_sst_AFTER =preferences.getString("pswd_from_AP","none");
  Serial.println(PSWD_sst_AFTER);
  preferences.end();
  
  preferences.begin("MQTT_pref",false);
  preferences.putString("MQTT_reg","NO");
  preferences.putString("token","none");
  preferences.end();


  
  preferences.begin("MQTT_pref",false);
  String MQTT_AFTER = preferences.getString("MQTT_reg","NO");
  Serial.println(MQTT_AFTER);
  String token_AFTER =preferences.getString("token","none");
  Serial.println(token_AFTER);
  preferences.end();

  
  delay(1000);
  digitalWrite(ledPin,LOW);
  digitalWrite(ledPin_button,LOW);
  delay(1000);
  
   ESP.restart();
  }
                     }





  
}
else
{
 digitalWrite(ledPin_button,LOW);//turn off the led
 timeBefore_pressed_buttonPin =millis();

//Fire led if Wifi is connected to Wifi
  


  if (!MQTTclient.connected() && notConnectedFirst) {
    Serial.println("Here: subscribing to NEW-DEV");
    mqttconnectFirst();
    }
  
  else if(!MQTTclient.connected() && notConnectedFirst==false) {
    Serial.println("??HERE??---> Connecting to MQTT");
    mqttconnect();
  }
  


  if(notConnectedFirst){
    
    MQTTclient.loop();

   if (millis()-lastMillisReg > 2000){
      Serial.println("Here: publishing on NEW-DEV topic");
      lastMillisReg = millis();
      
      char devId_and_Pin_char[devId_and_Pin.length()+1];
      devId_and_Pin.toCharArray(devId_and_Pin_char,devId_and_Pin.length()+1);
      
      //MQTTclient.publish(new_dev,devId_and_Pin_char);
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
      
      for(int k=0;k<5;k++)
      {
        MQTTclient.publish(Sub_reg_char,OK_char);
        }
      
      ESP.restart();
      
      
      }


    
    }

    
 else{

  getDHT();
  MQTTclient.loop();
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
    //(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)
    //MQTTclient.publish(Sub_ACT_char,startDrop_char);
    // rimuovi questa riga se alberto ti dice di non pubblicare
   //(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)
    }
    else if((char)ACTpayload[0]=='0'){

   ACTstate = 0;
   //(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)
   //MQTTclient.publish(Sub_ACT_char,stopDrop_char);
   // rimuovi ANCHE questa riga se alberto ti dice di non pubblicare
  //(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)(*)   
      }


    if (ACTstate ==1){
      
      digitalWrite(ledPin_water,HIGH);
      
      }
      else if (ACTstate==0){
        
      digitalWrite(ledPin_water,LOW);
      
        }


// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
if (((localTemp - localTemp_last_transmit) > 0.5) || ((localTemp - localTemp_last_transmit) < -0.5)) {
  //if (millis() - lastMillis > 2000) {
//[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
    localTemp_last_transmit = localTemp;
    
    String local_temp=String(localTemp);
    String local_hum=String(localHum);
    
    char local_temp_char[local_temp.length()+1];
     local_temp.toCharArray(local_temp_char,local_temp.length()+1);

    char local_hum_char[local_hum.length()+1];
     local_hum.toCharArray(local_hum_char,local_hum.length()+1);
    
    lastMillis = millis();
    
    
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

void wifiDisconnectedLoop(){

    WiFiClient client = server.available();   // listen for incoming clients
    
    delay(300);
    if(client){
      Serial.println("comm_app:"+String(comm_app));
      Serial.println("New client");
      String currentLine = "";                // make a String to hold incoming data from the client
      while (client.connected()) {            // loop while the client's connected
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();             // read a byte, then
          Serial.write(c);                    // print it out the serial monitor
          if (c == '\n') {                    // if the byte is a newline character

            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {

              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
              
              // the content of the HTTP response follows the header:
              if (comm_app == 0){
                delay(500);
              if (WiFi_test){
                client.print("!!OK!!");
                }
                else{
              client.print("!!NO!!");}
              }
              else if (comm_app == 1){
                delay(500);
                Serial.println("!NO...!");
                client.print("!!NO!!");
               comm_app = 3;
              
              }
              else if (comm_app == 2){
               delay(500);
              Serial.println("!OK...!");
              client.print("!!OK!!");
              comm_app = 3;
               
               }
               else if (comm_app ==3 ){
              delay(500);
              Serial.println("restarting...");
              client.print("!!END!!");
              client.println();
              delay(3000);
              ESP.restart();
                }

             // The HTTP response ends with another blank line:
             client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
          continue;
        }
      
      

      if (currentLine.startsWith("GET /?ssid=") ) {
     
        Serial.println("");
        String qssid;
          qssid = currentLine.substring(11, currentLine.indexOf('&')); //parse ssid
          Serial.println("Wi-fi SSID -->"+qssid);
          
          char qssid_char[qssid.length()+1];
          qssid.toCharArray(qssid_char,qssid.length()+1);
          
          Serial.println("");
          got_ssid_from_AP = true;
          
        String qpswd;
          qpswd = currentLine.substring(currentLine.lastIndexOf('=') +1, currentLine.lastIndexOf(' ')); //parse password
          Serial.println("Wi-fi PSWD -->"+qpswd);
          
         char qpswd_char[qpswd.length()+1];
          qpswd.toCharArray(qpswd_char,qpswd.length()+1);
          
          Serial.println("");
          got_pswd_from_AP =true;
         
         if ( got_pswd_from_AP == true && got_ssid_from_AP == true && comm_app !=  2){
                  //client.stop();
                  //Serial.println("client disconnected")
                  got_pswd_from_AP == false;
                  got_ssid_from_AP == false;
                  
          comm_app =1;
          Serial.println("CHAR: Wi-fi SSID -->"+String(qssid_char));
          Serial.println("CHAR: Wi-fi PSWD -->"+String(qpswd_char));

           WiFi.begin(qssid_char,qpswd_char);
           Serial.println("Connecting to WiFi...");
           
           // add here time constraint to exit while loop !!
           // FUCK FUCK FUCK ==> ADD EVENT !!!!!!!!
           
           start_WiFi_test = millis();
           bool WiFi_test = false;
           bool keep_testing = true;
           // ()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()
           while ( (millis()-start_WiFi_test < 20000) && keep_testing ) {
            // qui puoi cambiare durata del Wifi-test.
          // ()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()
            Serial.println(" test on wifi...");
                //Wifi Connect
                //Serial.print(".");

                if(WiFi.status() == WL_CONNECTED){

                  
                  Serial.println("WiFi-test: OK... it connected");
                  Serial.println("WiFi_test: "+String(WiFi_test));
                  
                  
                  
                  WiFi_test = true;
                  comm_app = 2;
                  keep_testing = false;
                  
                   }

                 
                  else{


                  
                  Serial.println("WiFi-test: NO.. it didn't connected");
                  Serial.println("SSID -->"+String(qssid_char)+" PSWD -->"+String(qpswd_char));
                  Serial.println("WiFi_test: "+String(WiFi_test));
                  comm_app = 1;
                  }

           }

           
             
              

             

              if (WiFi_test && WiFi.status() == WL_CONNECTED ) {
                Serial.println("Updating Wifi parameters...");
                
              //********************************************
              WifiConnectedDevice = true;
              preferences.begin("wifi_pref",false);
              //preferences.putBool("wifi_connected_once",WifiConnectedDevice);
              preferences.putString("ssid_from_AP",qssid);
              preferences.putString("pswd_from_AP",qpswd);
              preferences.end();
              //**********************************************
              Serial.println("!OK...DOWN!");
              //client.print("!!OK!!");
              //client.println();
              Serial.println("Connected to the WiFi network");
              digitalWrite(ledPin, HIGH);
              //Serial.println("IP-Address: "); 
              //Serial.println(WiFi.localIP()); 
              //delay(1000);
              //digitalWrite(ledPin, LOW);
              
              delay(1000);
              
              MQTTclient.begin("mqtt.aifarmer.du.cdr.mn",8883,net);
              //MQTTclient.onMessage(receivedREGCallback);
              MQTTclient.connect(devId,mqttUser,mqttPassword);
              
              for(int i=0;i<5 ;i++){
                Serial.println("publish: "+String(i));
                MQTTclient.publish(new_dev,devId_and_Pin_char);
                delay(500);
              }

              digitalWrite(ledPin, LOW);
              MQTTclient.disconnect();
              WiFi.disconnect();
              
              }

            

             

              
             }
          
          }
       }
    
}

 
  client.stop();
  Serial.println("client disconnected");
 
}
}



