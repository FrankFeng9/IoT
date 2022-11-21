//I Yizhe Feng, 000798153 certify that this material is my original work. No other person's work has been used without due acknowledgement. 
//I have not made my work available to anyone else.


// buffer space for ThingSpeak's MQTT broker
#define MQTT_MAX_PACKET_SIZE                4096

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

#include "mqtt_secrets.h"
#include "wifi_secrets.h"

// Wifi login details
char ssid[] = SECRET_SSID;                    
char pass[] = SECRET_PASS;                   

// interface to ThingSpeak MQTT interface
const char* mqttServer  = "mqtt3.thingspeak.com";
const uint16_t mqttPort = 1883;
char mqttUserName[] = SECRET_MQTT_USERNAME;      
char mqttPass[]     = SECRET_MQTT_PASSWORD;     
char clientID[]     = SECRET_MQTT_CLIENT_ID;    

// ThingSpeak ChannelID.
int  channelID      = 1947651;

// this is our MQTT topic 
String  mqttTopic   = "channels/" + String(channelID) + "/publish";

// WiFi client
WiFiClient client;                                 

// MQTT publish/subscribe client
PubSubClient mqttClient( client );


// MPU 6050 sensor object
Adafruit_MPU6050 mpu;



// ***************************************************************************
// function to setup MPU6050.  configures acceleration range, and filter bandwidth.
 void configureMPU6050() {

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
}


//write x, y, z acceleration data from the MPU6050 sensor to ThingSpeak channel
void updateThingSpeak(void){
     sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  String mqttData;

  if ( mqttClient.connect(clientID, mqttUserName, mqttPass)) {
    Serial.println("Connection made to MQTT broker!");
    
    // write x, y ,z data to field1 , 2 ,3
    mqttData = "&field1=" + String(a.acceleration.x) + "&field2=" + String(a.acceleration.y) + "&field3=" + String(a.acceleration.z);
    Serial.println("  Publish data=" + mqttData);

    // publish data
    if ( mqttClient.publish(mqttTopic.c_str(), mqttData.c_str() )) {
      Serial.println("  Publish was successful!");
    } else {
      Serial.println("  Publish failed..");
    }

    // disconnect from broker
    mqttClient.disconnect();
  } else {
    Serial.print("  Failed with state: ");
    Serial.println(mqttClient.state());
  }
}



// ****************************************************************************
void setup() {
  // start debug console
  Serial.begin(115200);

  Serial.println("Yizhe Feng 000798153");

  Serial.println("\nMQTT Lab\n");

  // connect to WiFi
  Serial.printf("\nConnecting to %s ", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");


  // initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");


  // set up accel sensor
  configureMPU6050();

  // set MQTT broker info
  mqttClient.setServer(mqttServer, mqttPort);


}

// ****************************************************************************
void loop() {

  //publish data to ThingSpeak using MQTT protocol
  updateThingSpeak();
  
  // MQTT libraries need this
  mqttClient.loop();

  //delay 5s to publis data
  delay(5000);
}