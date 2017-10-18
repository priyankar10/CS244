#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

String deviceName = "CS244";

// WiFi settings
const char *ssid = "UCInet Mobile Access";
// Host name - Server on local machine
String host ="169.234.52.161"; // change the host as per the local IP address
WiFiClient client;

char MAC_char[18]="";
void printMacAddress()
{
    byte mac[6];
    WiFi.macAddress(mac);
    for (int i = 0; i < sizeof(mac); ++i)
    {
        sprintf(MAC_char, "%s%02x:", MAC_char, mac[i]);
    }

    Serial.print("Mac address : ");
    Serial.print(MAC_char);

    WiFi.begin(ssid);
    //, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    // Print the IP address
    Serial.println(WiFi.localIP());

}

void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
    
  Serial.begin(115200);
  Serial.println("Program started");
  printMacAddress();
}

void loop()
{

  String data = "Assignment 1 - CS244";
  String data1 = "  By Priyanka Ravi, Abhinaya Dasarathy";
  // Using HTTP Post to transmit data to the server
 if(WiFi.status() == WL_CONNECTED){
   HTTPClient http;
   Serial.print("[HTTP] begin...\n");
   http.begin("http://"+host+":80/assignment1.php");
   Serial.println("[HTTP] POST...\n");
   http.addHeader("Content-Type","text/plain");
   int httpCode = http.POST(data + data1);
   if(httpCode > 0 ){
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      if(httpCode == HTTP_CODE_OK) {
                   String payload = http.getString();
                   Serial.println(payload);
               
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
           
      }
    }
  http.end();
 }
 else
 {
   Serial.println("Error in connecting to Wifi");
 }
 delay(100);
  
  // You can write your logic here
  // Below are the LED samples from the hands-on

  // turn the LED on (HIGH is the voltage level)
 // digitalWrite(LED_BUILTIN, HIGH);
  // wait for a second
  //delay(1000);
  // turn the LED off by making the voltage LOW
  //digitalWrite(LED_BUILTIN, LOW);
   // wait for a second
  //delay(1000);
}