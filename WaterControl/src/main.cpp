
// Water Control System-  controls the pump that maintains sufficient water in the tank                                                   
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "wifi_secrets.h"
#include "mqtt_secret.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// digital input pin definitions
#define E_STOP D6
#define HIGH_LEVEL D7
#define WATER_LEVEL_SENSOR A0
#define LED_GREEN D2
#define LED_RED D5
//led state
#define LED_ON 1
#define LED_OFF 0
//states indicators
String eStopState = "No";
String highTankState = "No";
volatile bool faultIndicator;
bool pumbOn = false;
//water tank level
int waterLevel;
//setup tempture  sensor
const int oneWireBus = D3;
OneWire onewaire(oneWireBus);
DallasTemperature DS18B20(&onewaire);
DeviceAddress DS18B20Adrs;
// Wifi client
char ssid[] = SECRET_SSID; 
char pass[] = SECRET_PASS; 
WiFiClient client;
// MQTT publish/subscribe client
PubSubClient mqttClient(client);
const char *mqttServer = SERVER;
const uint16_t mqttPort = PORT;
char mqttUserName[] = "";
char mqttPass[] = "";
char clientID[] = ID;
// MQTT topic
String mqttTempTopic = "iot3721/WaterControl/temp";
String mqttTankLevelTopic = "iot3721/WaterControl/water_level";
String mqtteStopTopic = "iot3721/WaterControl/e_stop";
String mqttHtankStateTopic = "iot3721/WaterControl/high_level";
String mqttremoteTopic = "iot3721/WaterControl/remote_stop";

// read temperature function return temperature
float readTemperature()
{
  float fTemp = 0;
  // ask DS18B20 for the current temperature
  if (DS18B20.requestTemperatures())
  {
    // fetch the temperature
    fTemp = DS18B20.getTempCByIndex(0);
  }
  else
  {
    Serial.println("can't read temperature form DS18B20");
  }
  return fTemp;
}

//update MQTT function
void updateMQTT()
{
  // if not connected, try to reconnect
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect(clientID, mqttUserName, mqttPass))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  if (mqttClient.connected())
  {
    // last publish time
    static unsigned long lastTime;
    // subscribe to topic
   mqttClient.subscribe("iot3721/WaterControl/remote_stop");
    // 5 seconds since last update
    if (millis() - lastTime > 5000)
    {
      lastTime = millis();
      // read temperature
      String temp = String(readTemperature());
      //read water level, convert to percentage
      waterLevel = analogRead(WATER_LEVEL_SENSOR) * 100 / 1024;
      // publish topic
      mqttClient.publish(mqttTempTopic.c_str(), (temp + "°C").c_str());
      mqttClient.publish(mqttTankLevelTopic.c_str(), (String(waterLevel) + "%").c_str());
      mqttClient.publish(mqtteStopTopic.c_str(), eStopState.c_str());
      mqttClient.publish(mqttHtankStateTopic.c_str(), highTankState.c_str());
      Serial.println("topics published");                             
    }
  }   
}
//get the subscrition topic
void callback(char *topic, byte *payload, int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String message;
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();
  // subscript topic
  if (String(topic) == mqttremoteTopic)
  {
    Serial.print("Remote Stop");
    if (message == "1")
    {
      Serial.println("on");
      faultIndicator = true;
    }
  }
}
// ISR fcuntion for eStop
void IRAM_ATTR isreStop()
{
  faultIndicator = true;
  eStopState = "YES";
}
// ISR function for high water level
void IRAM_ATTR isrHighWaterLevel()
{
  faultIndicator = true;
  highTankState = "YES";
}
//setup
void setup()
{
  Serial.begin(115200);
  pinMode(E_STOP, INPUT_PULLUP);
  pinMode(HIGH_LEVEL, INPUT_PULLUP);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  // turn led off at startup
  digitalWrite(LED_GREEN, LED_OFF);
  digitalWrite(LED_RED, LED_OFF);
  //attach ISR 
  attachInterrupt(digitalPinToInterrupt(E_STOP), isreStop, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HIGH_LEVEL), isrHighWaterLevel, CHANGE);
  DS18B20.begin();
  //connect to wifi
  Serial.printf("\nConnecting to %s ", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(5000);
    Serial.print(".");
  }
  Serial.println(" connected");
  //set up connection to mqtt broker
  mqttClient.setServer(mqttServer, mqttPort);
  //register callback function
  mqttClient.setCallback(callback);
  //pumb off at startup
  pumbOn = false;
  
}
void loop()
{ 
  //update mqtt
  updateMQTT();
  mqttClient.loop();
  // Turn on the red LED if fault indicator active or temperature is above 30°C, turn off the green LED.
  if (faultIndicator || readTemperature() >  30)
  {
    digitalWrite(LED_RED, LED_ON);
    digitalWrite(LED_GREEN, LED_OFF);
    pumbOn = false;
  }
  else
  { 
    // Turn on the pump when the tank level drops below 20%.
    if ((waterLevel < 20) & !pumbOn & !faultIndicator)
    {
      pumbOn = true;
      digitalWrite(LED_GREEN, LED_ON);
    }
    // Turn off the pump when the tank level is above 90%.
    if (pumbOn & (waterLevel > 90))
    {
      digitalWrite(LED_GREEN, LED_OFF);
      pumbOn = false;
    }
  }

}