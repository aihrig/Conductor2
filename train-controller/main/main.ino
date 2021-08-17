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
long lastReconnectAttempt = 0;
int pingInterval = 30 * 1000; // Ping/status pub message ever 30 seconds
long lastPing = pingInterval; // Ping on start

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

// PubSubClient / PubNub setup
const char *mqttServer = "mqtt.pndsn.com";
const int mqttPort = 1883;
PubSubClient client(MQTTclient);
String clientID;
String channelName;

// Callback for message processing
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("Topic: " + String(topic));
  String payload_buff;
  for (int i = 0; i < length; i++)
  {
    payload_buff = payload_buff + String((char)payload[i]);
  }
  Serial.print("Raw payload -->");
  Serial.print(payload_buff); // Print out messages.
  Serial.println("<--");

  // Deserialize/parse the JSON document
  DeserializationError error = deserializeJson(doc, payload_buff);
  
  // Test if parsing succeeds, and if so, process accordingly
  if (error)
  {
    String errorInfo = "deserializeJson() failed: ";
    errorInfo += String(error.c_str());
    Serial.println(errorInfo);
  }
  else
  {
    serializeJson(doc, Serial); // DEBUG: write doc to serial
    const String command = doc[0]["command"];

    if (command != "null")
    {
      hst01.handleCommandMessages(doc);
    }
  }
}

// PubSub client helper to reconnect if connection is lost
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

  // Load configuration data (from SPIFFS currently)
  // TODO: Look into using AutoConnect library to store configuration
  //       data from captive portal initial setup.
  if (!config.loadParams())
  {
    Serial.println("### UNABLE TO LOAD CONFIGURATION DATA - HALTING ###");
    return;
  }
  else
  {
    channelName = config.channel();
    clientID = config.pubkey() + "/" + config.subkey() + "/" + "hst01";
    Serial.println("Configuration data loaded successfully!");
  }

  // Start hst01 Train
  hst01.begin();

  // Respond to http request for root page
  Server.on("/", rootPage);

  // AutoConnect configuration portal check, start pubnub if WiFi is ready
  if (portal.begin())
  {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
    // DEBUG: Remove or standardize message with a logger
    Serial.println("AutoConnect Portal has been started.");

    // Configure pubsub client for PubNub
    lastReconnectAttempt = 0;
    lastPing = 0;
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
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
  { // PubSub client connected to PubNub, process accordingly.
    client.loop();

    // Create and send status message each pingInterval
    long now = millis();
    if (now - lastPing > pingInterval) {
      // TODO: Make message dynamic based on train object's name.
      client.publish(channelName.c_str(), "{\"status\":\"HST01 Connected\"}");
      lastPing = now;
    }
  }

  // TODO: Consider implementing direct REST server (aREST)
  // Handle incoming web requests
  portal.handleClient();

  
}