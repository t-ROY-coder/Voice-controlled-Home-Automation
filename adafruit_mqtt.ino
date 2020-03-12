#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "*****" // WiFi SSID
#define WIFI_PASS "*****" // WiFi Network's Password

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "*****" // Your Adafruit.io username
#define MQTT_PASS "*****" // Your channel's AIO Key

//Set up MQTT and WiFi clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS); // create a MQTT Client for Adafruit

//Set up the feed you're subscribing to
Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/test"); // Specify the name of the feed you created ("/f/<Your feed name>")
//This feed should be subscribed to.


void setup()
{
  Serial.begin(115200);
  Serial.print("\n\nConnecting to Wifi... "); //Connect to WiFi.
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  Serial.println("OK!");
  mqtt.subscribe(&onoff); // Subscribe to the feed.

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  MQTT_connect();
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000))) //Will return true if the subscribed topic gets an updation
  {
    //If we're in here, a subscription updated...
    if (subscription == &onoff)
    {
      Serial.print("onoff: ");
      Serial.println((char*) onoff.lastread);
     
      if (!strcmp((char*) onoff.lastread, "it's ON"))
      {
       
        digitalWrite(LED_BUILTIN, LOW);
      }
      else
      {
        digitalWrite(LED_BUILTIN, HIGH);
      }
    }
  }

  // ping the server to keep the mqtt connection alive
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }
}



void MQTT_connect()
{
  int8_t ret;
  if (mqtt.connected())
  {
    return;   // Return if already connected.
  }
  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // mqtt.connect() will return 0 if connected.
  {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0)
       {
         while (1); //Unconditionally block program execution. Hard RESET Required.
       }
  }
  Serial.println("MQTT Connected!");
}
