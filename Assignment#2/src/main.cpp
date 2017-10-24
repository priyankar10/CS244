#include "Arduino.h"
#include <Wire.h>
#include "MAX30105.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

MAX30105 particleSensor;
int currentNo = 0;

String data = "";
int noOfSamples=0;
HTTPClient http;
int httpCode;
// WiFi settings
const char *ssid = "UCInet Mobile Access";
// Host name - Server on local machine
String host ="ec2-35-167-126-243.us-west-2.compute.amazonaws.com"; // change the host as per the local IP address
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
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Program started");
  printMacAddress();

  Serial.println("Initializing");
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }

  //void setup(byte powerLevel = 0x1F, byte sampleAverage = 4, byte ledMode = 3, int sampleRate = 400, int pulseWidth = 411, int adcRange = 4096);
  particleSensor.setup(0xFF,4, 3, 50, 411, 4096); //Configure sensor. Use 6.4mA for LED drive
  
    //LED Pulse Amplitude Configuration
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    //Default is 0x1F which gets us 6.4mA
    //powerLevel = 0x02; // 0.4mA - Presence detection of ~4 inch
    //powerLevel = 0x1F, 6.4mA - Presence detection of ~8 inch
    //powerLevel = 0x7F, 25.4mA - Presence detection of ~8 inch
    //powerLevel = 0xFF, 50.0mA - Presence detection of ~12 inch
  
    particleSensor.setPulseAmplitudeRed(0xFF);
    particleSensor.setPulseAmplitudeIR(0xFF);
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED) {

      //Getting sensor values
      uint32_t red = particleSensor.getRed();
      uint32_t IR = particleSensor.getIR();

      //Appending to data in the form of lines red,IR, accumulating samples in 100
      data += red;
      data +=  ",";
      data += IR;
      data += "\n";

      noOfSamples++;
      if(noOfSamples==100){
        Serial.println(currentNo);
        Serial.print("[HTTP] begin...\n");
        http.begin("http://"+host+":80/assignment1.php");
        Serial.println("[HTTP] POST...\n");
        http.addHeader("Content-Type","text/plain");
        
        httpCode = http.POST(data);   //Send the request
        Serial.println(httpCode);
        http.end();  //Close connection

        if(currentNo == 60) {
        	particleSensor.shutDown();
        	return;
        }

        currentNo++; //For next 100 samples
        data="";
        noOfSamples=0;
      }
      
    } else {
        Serial.println("Error in WiFi connection");
    }
}