#include "SparkFunLIS3DH.h"
#include "Wire.h"
#include "MAX30105.h"
#include "SPI.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//Accelerometer in I2C mode
LIS3DH myIMU(I2C_MODE, 0x19);

//particle sensor
MAX30105 particleSensor;
unsigned long start_time = 0; // for time stamp
float interval_time = 0;
String deviceName = "CS244";
String data = "";
int noOfSamples=0;
HTTPClient http;
int currentNo = 0;
int httpCode;
// WiFi settings
const char *ssid = "UCInet Mobile Access";
// Host name - Server on local machine
String host ="169.234.117.123"; // change the host as per the local IP address
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
  Serial.begin(115200);
  delay(1000); 
 
  printMacAddress();
    // configure the IMU
   // Accelerometer setup
   myIMU.settings.accelSampleRate = 50;  //Hz.  Can be: 0,1,10,25,50,100,200,400,1600,5000 Hz
   myIMU.settings.accelRange = 2; 
   myIMU.settings.adcEnabled =0;
   myIMU.settings.tempEnabled =0;
   myIMU.settings.xAccelEnabled = 1;
   myIMU.settings.yAccelEnabled = 1;
   myIMU.settings.zAccelEnabled = 1;

    // PPG sensor setup
    if (!particleSensor.begin(Wire,I2C_SPEED_FAST))
       {
      Serial.println("MAX30105 was not found. Please check wiring/power.");
      while (1);
    }
    particleSensor.setup(); 
    particleSensor.setSampleRate(50);
    particleSensor.setFIFOAverage(1);

   //Configure sensor. Use 6.4mA for LED drive
    particleSensor.setPulseAmplitudeRed(0x1F);
    particleSensor.setPulseAmplitudeIR(0x1F);
    
    //start the Accelerometer
    myIMU.begin();
    Serial.println();
    start_time = millis();
  
}



void loop()
{   
  uint32_t ir;
  uint32_t red;
  if (WiFi.status() == WL_CONNECTED) {
        //a new method is added in MAX30105.cpp to get the IR and RED values for the same instant
        particleSensor.getIRRed(&ir,&red); 
        //Appending to data in the form of lines red,IR, accumulating samples in 100
        data = data + String(float(ir)) + ',' + String(float(red)) + ',' +String(myIMU.readFloatAccelX()) +','+String(myIMU.readFloatAccelY())+ ',' + String(myIMU.readFloatAccelZ()) +'\n';
        noOfSamples++;
        if(noOfSamples==100){
          http.begin("http://"+host+":80/assignment6.php");
          http.addHeader("Content-Type","text/plain");
          httpCode = http.POST(data);   //Send the request
          Serial.println(httpCode);
            //http.end();  //Close connection
          data="";
          noOfSamples=0;
        }
           
  } else {
            Serial.println("Error in WiFi connection");
    } 
} 





