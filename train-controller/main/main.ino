// #include <aREST.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <AutoConnect.h>
#define PubNub_BASE_CLIENT WiFiClient
#include <PubNub.h>
#include "library/Train/Train.h"

// This is where the pub/sub keys will be stored once saved
#define PARAM_FILE "/param.json"

// TODO: Add fields to AutoConnect setup
const static char pubkey[] = "pub-c-444d9350-c274-45cd-ab65-007aba298db9";
const static char subkey[] = "sub-c-17598d96-f23b-11e6-b7fd-02ee2ddab7fe";
const static char channel[] = "hello_world";

// Set up train with a name and left and right headlight LED pins.
Train hs01("High Speed 01", 27, 12);

// Instantiate JSON document object with 1024 bytes of room
DynamicJsonDocument doc(1024);

// Setup AutoConnect portal/web server
WebServer Server;
// AutoConnect Portal(Server);
AutoConnect portal;
String pub_key;
String sub_key;

// This function will load the params file from the ESP32 file system
String loadParams()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return "";
  }

  File paramFile = SPIFFS.open(PARAM_FILE);
  if (!paramFile)
  {
    Serial.println("Failed to open file for reading");
    return "";
  }
}

// Respond to root http request
void rootPage()
{
  char content[] = "Server running";
  Server.send(200, "text/plain", content);
}

void setup()
{
  // Start serial output
  delay(500);
  Serial.begin(115200);

  // Start hs01 Train
  hs01.begin();
  hs01.fullSteamAhead();

  // Respond to http request for root page
  Server.on("/", rootPage);

  // AutoConnect configuration portal check, start pubnub if WiFi is ready
  if (portal.begin())
  {
    PubNub.begin(pubkey, subkey); // Start PubNub.
    Serial.println("PubNub is set up.");
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  // hs01.ready();
  // Serial.println(hs01.name());
  // hs01.lightsOn();
  // hs01.fullSteamAhead();
}

void loop()
{

  // TODO: move to separate function/file
  // Subscribe to train control channel on PubNub
  PubSubClient *sclient = PubNub.subscribe(channel);
  if (sclient != 0)
  {
    char nextChar;
    String message;
    while (sclient->wait_for_data())
    {
      nextChar = sclient->read();
      message += nextChar;
    }
    sclient->stop();

    // Deserialize/parse the JSON document
    DeserializationError error = deserializeJson(doc, message);

    // Test if parsing succeeds, and if so, process accordingly
    if (error)
    {
      String errorInfo = "deserializeJson() failed: ";
      errorInfo += String(+error.c_str());
      Serial.println(errorInfo);
    }
    else
    {
      // serializeJson(doc, Serial); // DEBUG: write doc to serial
      const char *message = doc[0]["message"];
      const char *data = doc[0]["data"];
      String msg = "Incoming message --> ";
      Serial.print(msg);
      Serial.println(message);
      msg = "Incoming data --> ";
      Serial.print(msg);
      Serial.println(data);
    }
  }

  // TODO: Consider implementing direct REST server (aREST)
  // Handle incoming web requests
  portal.handleClient();
}