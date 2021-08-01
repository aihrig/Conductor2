// #include <aREST.h>
#include <Arduino.h>
#include <String>
#include <WiFi.h>
#include <Wire.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <AutoConnect.h>
#include "src/Config/Config.h"
#include "src/Train/Train.h"

// This is where the pub/sub keys will be stored once saved
Config config("/env.json");

// Additional configuration items
long lastPing = 0;
long lastReconnectAttempt = 0;
int pingInterval = 30 * 1000; // Ping/status pub message ever 30 seconds

// Set up train with a name and left and right headlight LED pins.
Train hst01("High Speed 01", 27, 12);

// Instantiate JSON document object with 1024 bytes of room
DynamicJsonDocument doc(1024);

// Setup AutoConnect portal/web server
WebServer Server;
// AutoConnect Portal(Server);
AutoConnect portal;

// Set up a client for network communications
WiFiClient MQTTclient;

// PubSubClient / PubNub
const char *mqttServer = "mqtt.pndsn.com";
const int mqttPort = 1883;
PubSubClient client(MQTTclient);
String clientID;
String channelName;
void callback(char *topic, byte *payload, unsigned int length)
{
  String payload_buff;
  for (int i = 0; i < length; i++)
  {
    payload_buff = payload_buff + String((char)payload[i]);
  }
  Serial.println(payload_buff); // Print out messages.
}

boolean reconnect()
{
  if (client.connect(clientID.c_str()))
  {
    client.subscribe(channelName.c_str()); // Subscribe to channel.
  }
  return client.connected();
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

  // Load configuration data
  if (!config.loadParams())
  {
    Serial.println("### UNABLE TO LOAD CONFIGURATION DATA ###");
    return;
  }
  else
  {
    channelName = config.channel();
    clientID = config.pubkey() + "/" + config.subkey() + "/" + "hst01";
    Serial.println("Configuration data loaded");
  }

  // Start hst01 Train
  hst01.begin();

  // Respond to http request for root page
  Server.on("/", rootPage);

  // AutoConnect configuration portal check, start pubnub if WiFi is ready
  if (portal.begin())
  {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
    Serial.println("portal.begin() has been called");

    client.setServer(mqttServer, mqttPort); // Connect to PubNub.
    client.setCallback(callback);
    lastReconnectAttempt = 0;
    lastPing = 0;
    Serial.println("PubNub has been configured.");

    // Turn on the built-in LED to signal that app is running
    // and WiFi is working.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
  }

}

void loop()
{
  // Serial.println("Looping...");

  // TODO: move to separate function/file
  if (!client.connected())
  {
    long now = millis();
    if (now - lastReconnectAttempt > 5000)
    { // Try to reconnect.
      lastReconnectAttempt = now;
      if (reconnect())
      { // Attempt to reconnect.
        lastReconnectAttempt = 0;
      }
    }
  }
  else
  { // Connected.
    client.loop();

    long now = millis();
    if (now - lastPing > pingInterval) {
      client.publish(channelName.c_str(), "HST01 Connected"); // Publish message.
      lastPing = now;
    }
  }

  // TODO: Consider implementing direct REST server (aREST)
  // Handle incoming web requests
  portal.handleClient();

  /*
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
      errorInfo += String(error.c_str());
      Serial.println(errorInfo);
    }
    else
    {
      // serializeJson(doc, Serial); // DEBUG: write doc to serial
      const String message = doc[0]["message"];
      const String data = doc[0]["data"];
      String msg = "Incoming message -->";
      Serial.print(msg);
      Serial.print(message);
      Serial.println("<--");
      msg = "Incoming data -->";
      Serial.print(msg);
      Serial.print(data);
      Serial.println("<--");

      if ((message == "headlights") && (data == "on"))
      {
        Serial.println("Turning on headlights");
        hst01.headlightsOn();
      }
      if ((message == "headlights") && (data == "off"))
      {
        Serial.println("Turning off headlights");
        hst01.headlightsOff();
      }

      if ((message == "motor") && (data == "fullsteamahead"))
      {
        hst01.fullSteamAhead();
      }
    }
  }
*/
}