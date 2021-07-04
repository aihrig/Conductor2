# 1 "d:\\Development\\MyProjects\\BrickCity\\Conductor2\\train-controller\\main.ino"
# 1 "d:\\Development\\MyProjects\\BrickCity\\Conductor2\\train-controller\\main.ino"
// #include <aREST.h>
# 3 "d:\\Development\\MyProjects\\BrickCity\\Conductor2\\train-controller\\main.ino" 2
# 4 "d:\\Development\\MyProjects\\BrickCity\\Conductor2\\train-controller\\main.ino" 2
# 5 "d:\\Development\\MyProjects\\BrickCity\\Conductor2\\train-controller\\main.ino" 2

# 7 "d:\\Development\\MyProjects\\BrickCity\\Conductor2\\train-controller\\main.ino" 2
# 8 "d:\\Development\\MyProjects\\BrickCity\\Conductor2\\train-controller\\main.ino" 2
# 9 "d:\\Development\\MyProjects\\BrickCity\\Conductor2\\train-controller\\main.ino" 2
#define PubNub_BASE_CLIENT WiFiClient
# 11 "d:\\Development\\MyProjects\\BrickCity\\Conductor2\\train-controller\\main.ino" 2
// #include "Train/Train.h"

// TODO: Add fields to AutoConnect setup
const static char pubkey[] = "pub-c-444d9350-c274-45cd-ab65-007aba298db9";
const static char subkey[] = "sub-c-17598d96-f23b-11e6-b7fd-02ee2ddab7fe";
const static char channel[] = "hello_world";

// Instantiate train with a name and left and right head
// headlight LED pins.
// Train hs01("High Speed 01", 27, 12);

// Instantiate JSON document object with 1024 bytes of room
DynamicJsonDocument doc(1024);

// Setup AutoConnect portal/web server
WebServer Server;
AutoConnect Portal(Server);

// Function to respond to root http request
void rootPage()
{
  char content[] = "Server running";
  Server.send(200, "text/plain", content);
}

void setup()
{
  // Start serial output
  delay(1000);
  Serial.begin(115200);

  // Respond to http request for root page
  Server.on("/", rootPage);

  // AutoConnect configuration portal check, start pubnub if WiFi is ready
  if (Portal.begin())
  {
    PubNub.begin(pubkey, subkey); // Start PubNub.
    Serial.println("PubNub is set up.");
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }

  // Serial.println(hs01.name());
  // hs01.lightsOn();
  // hs01.fullSteamAhead();
}

void loop()
{

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
  Portal.handleClient();
}
