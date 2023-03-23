//YouHaveMail  this app represents a mail notification system, it will email users when the mail box is opened(button press) and back to sleep after sent notification

#include <Arduino.h>
#include <ESP8266WiFi.h> 
#include <secrets.h>
#include "ThingSpeak.h" 
#include <ESP_EEPROM.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//access credentials for WiFi network. 
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

//wifi connection 
WiFiClient  client;
WiFiUDP ntpUDP;

// ThingSpeak channel ID and write API key.
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

//Month List
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

//specify the time server pool and the offset, update interval, using EST time zone
NTPClient timeClient(ntpUDP, "0.ca.pool.ntp.org", -18000, 60000);

//function to send notification to ThinkSpeak channel if the last update was more than 1 mins
void mailNotification()
{
   //last time the data was sent
  time_t  lastUpdatetime;

  //read the data from the EEPROM
  EEPROM.get(0, lastUpdatetime);

 //compare the last update to the current time from the NTP server to determine if 60 seconds passed
 if (timeClient.getEpochTime() - lastUpdatetime > 60)
 {
    //log 1 to ThingSpeak
    int x = ThingSpeak.writeField(myChannelNumber, 1, 1, myWriteAPIKey);
    if (x == 200)
    {
      //update success, write the current time to the EEPROM
      lastUpdatetime = timeClient.getEpochTime();

      // Get a time structure
      struct tm *ptm = gmtime((time_t *)&lastUpdatetime);
      //date in month
      long monthDay = ptm->tm_mday;
      //get the month name from the array
      String currentMonthName = months[(ptm->tm_mon + 1) - 1];
      //get the year
      long currentYear = ptm->tm_year + 1900;
      //get the time  
      String formattedTime = timeClient.getFormattedTime();
      //print the date and time
      Serial.println("ThinkSpeak updated at " + currentMonthName + " " + String(monthDay) + ", " + String(currentYear) + " " + formattedTime);
      //write the current time to the EEPROM
      EEPROM.put(0, lastUpdatetime); 
       EEPROM.commit();    
      //add a delay  
      delay(1000);

    }
    else
    {
       Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }   
}

void setup() {

//print oput app info when start
Serial.begin(115200); 

while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

WiFi.mode(WIFI_STA); 
//Initialize ThingSpeak
ThingSpeak.begin(client); 
// The begin() call is required to initialise the EEPROM library
EEPROM.begin(16);

// connect to WiFi
Serial.printf("\nConnecting to %s ", ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED)
{
  delay(500);
  Serial.print(".");
}
Serial.println(" connected");

}

void loop() {
  
  //update the time client
  timeClient.update();
  //call the mailNotification function 
  mailNotification();
  //put the ESP8266 to sleep
  ESP.deepSleep(0);
 
}






    
    
 
     
  
