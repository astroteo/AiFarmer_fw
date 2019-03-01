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


// Led pins
  //Wifi connected device 
  int ledPin = 12;
  bool WifiConnectedDevice = false;

// Set-up for DHT sensor
#define DHTPIN 27 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);
float localHum = 0;
float localTemp = 0;

// Preferences (permanently flashed parameters.)
Preferences preferences;

// Connection-type & parameters


   //WiFi mode {Script-core -->  :) look @ void loop()}
   bool wifi_connected = false;

   //DEBUG: Wifi-mode Directly
   bool WiFiDirectConnect = false;
  bool TrueTest = true;
  const char* ssid     = "Telecom-47442901";//      "peace" "supercampus" 
  const char* pswd = "baiguerabaiguerabaiguera";//  ; ;"lucieombre""superfaggin"
  
  //AP-mode                
  //const char *ssidESP32 = "ESP32ap";//const char *passwordESP32 = "12345678"; <-- useless to have Psw on device!!
  #define AP_SSID  "esp32" 
  bool requestPswd = true;
  const char* ssidFromAP     = " ";// "Telecom-47442901"   "peace"
  const char* passwordFromAP = " ";//  "baiguerabaiguerabaiguera"; ;"lucieombre"

  const char* ssid_st_char    = " ";// "Telecom-47442901"   "peace"
  const char* pswd_st_char    = " ";//  "baiguerabaiguerabaiguera"; ;"lucieombre"

  //AP-mode --> Wifi-mode
  static volatile bool got_ssid_from_AP = false;
  static volatile bool got_pswd_from_AP = false;

  String WiFiSSID;
  String WiFiPswd;

  

  unsigned long start_WiFi_test;
  static volatile bool WiFiGood = false;


WiFiServer server(80);

void setup() {

Serial.begin(115200);
delay(5000);// DEBUG: time to open serial for 



// pinMode setup
 pinMode (ledPin, OUTPUT);
 delay(5000);


 


//******************************************************************************
// begin preferences --> acces r/w permanent memory of ESP32
preferences.begin("wifi_pref",false);
Serial.println("...get_preferences");
//bool wifi_connected_once = preferences.getBool("wifi_connected_once",false);
String ssid_st = preferences.getString("ssid_from_AP","none");
String pswd_st = preferences.getString("pswd_from_AP","none");
preferences.end();

preferences.begin("pin",false);
String PIN_st = preferences.getString("pin");
preferences.end();




Serial.println("ssid from AP:"+ssid_st);
char ssid_st_char[ssid_st.length()+1];
     ssid_st.toCharArray(ssid_st_char,ssid_st.length()+1);


Serial.println("pswd from AP:"+pswd_st);
char pswd_st_char[pswd_st.length()+1];
     pswd_st.toCharArray(pswd_st_char,pswd_st.length()+1);
//*********************************************************************************
     
bool wifi_connected_once = false;
if (ssid_st.equals("none") ||  pswd_st.equals("none") )
{ }
else
{ wifi_connected_once = true;}


Serial.println("This device has already been connected to the internet ??");
Serial.println(String(wifi_connected_once));



// !! replacED: WiFiDirectConnect --> wifi_connected_once, remove: TrueTest
if (wifi_connected_once)
{

  Serial.println("???? ENTERED HERE ???");
  WiFi.begin(ssid_st_char, pswd_st_char);
  while (WiFi.status() != WL_CONNECTED) {
                //Wifi Connect
                Serial.println("Connecting to WiFi..");
                }
             
              Serial.println("Connected to the WiFi network");
              WifiConnectedDevice = true;
              
   Serial.println("IP-Address: "); 
   Serial.println(WiFi.localIP()); 
   delay(1000);

   wifi_connected = true;
   }
 
else {

  Serial.print("Configuring access point...\n");
 //WiFi.mode(WIFI_MODE_APSTA);
 WiFi.softAP(AP_SSID);
 WiFi.softAPsetHostname(AP_SSID);
 WiFi.softAPenableIpV6();//enable ap ipv6 here
 
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

   Serial.println("Doing the connected Loop");
 

  if (WiFi.status() == WL_CONNECTED){
    
    digitalWrite(ledPin, HIGH);
    Serial.println("connected to Wifi");
  }  

}

void wifiDisconnectedLoop(){

    
    WiFiClient client = server.available();   // listen for incoming clients

    delay(300);
    if(client){

      Serial.println("New client");
      String currentLine = "";                // make a String to hold incoming data from the client
      String fullString = "";
      
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
              client.print("Hello I'm MATTEO talking to you from ESP32... your connection is in AP-MODE");

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
          
          
          
          if ( got_pswd_from_AP == true && got_ssid_from_AP == true){

          
          Serial.println(" end of AP proceadure");
          
          Serial.println("CHAR: Wi-fi SSID -->"+String(qssid_char));
          Serial.println("CHAR: Wi-fi PSWD -->"+String(qpswd_char));
          
           WiFi.begin(qssid_char,qpswd_char);
           Serial.println("Connecting to WiFi...");
           
           // add here time constraint to exit while loop !!
           // FUCK FUCK FUCK ==> ADD EVENT !!!!!!!!
           
           start_WiFi_test = millis();
           bool WiFi_test = false;
           
           while ( millis()-start_WiFi_test < 20000) {

            Serial.println(" test on wifi...");
                //Wifi Connect
                //Serial.print(".");

                if(WiFi.status() == WL_CONNECTED){

                  Serial.println("WiFi-test: OK \n... it connected");
                  WiFi_test = true;
                  Serial.println(String(WiFi_test));
                  }

                 
                }

           
             
              Serial.println("Connected to the WiFi network");
              digitalWrite(ledPin, HIGH);

             
              
              
              Serial.println("IP-Address: "); 
              Serial.println(WiFi.localIP()); 
              delay(10000);

              if (WiFi_test) {
                Serial.println("Updating Wifi parameters...");
              //********************************************
              WifiConnectedDevice = true;
              preferences.begin("wifi_pref",false);
              //preferences.putBool("wifi_connected_once",WifiConnectedDevice);
              preferences.putString("ssid_from_AP",qssid);
              preferences.putString("pswd_from_AP",qpswd);
              preferences.end();
              
              
              //**********************************************
              }
              //else{ DO THE POST TO APP... ASKING AGAIN FOR WIFI}


          
          
          
         ESP.restart();
         }
         
         }
          
          }

    }

     
    
    }
    }


              
              
       

  









