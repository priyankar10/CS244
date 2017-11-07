#include "SparkFunLIS3DH.h"
#include "Wire.h"
#include "SPI.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// constructed with parameters for SPI and cs pin number
//chip select is connected to GPIO pin 0
LIS3DH myIMU(SPI_MODE,0); 

String deviceName = "CS244";
String data = "";
int noOfSamples=0;
HTTPClient http;
int httpCode;
// WiFi settings
const char *ssid = "UCInet Mobile Access";
// Host name - Server on local machine
String host ="169.234.49.71"; // change the host as per the local IP address
WiFiClient client;

char MAC_char[18]="";
//to setup wifi and begin wifi
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
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(1000); 
  Serial.println("Processor came out of reset.\n");
  printMacAddress();
  // configure the IMU
  //default frequency = 50Hz
  myIMU.begin();}


void loop()
{   

    if (WiFi.status() == WL_CONNECTED) {
    
          //Getting accelerometer values
          float x_value = myIMU.readFloatAccelX();
          float y_value = myIMU.readFloatAccelY();
          float z_value = myIMU.readFloatAccelZ();
          //Appending to data in the form of x ,y z values
          data += x_value;
          data +=  ",";
          data += y_value;
          data += ",";
          data += z_value;
          data += "\n";
          noOfSamples++;
          
          if(noOfSamples==100){
            
            Serial.print("[HTTP] begin...\n");
            http.begin("http://"+host+":80/assignment4.php");
            Serial.println("[HTTP] POST...\n");
            http.addHeader("Content-Type","text/plain");
            
            httpCode = http.POST(data);   //Send the request
            Serial.println(httpCode);
            http.end();  //Close connection
            data="";
            noOfSamples=0;
           
          }
          
        } else {
            Serial.println("Error in WiFi connection");
        } 
}