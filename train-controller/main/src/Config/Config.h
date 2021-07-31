/* Config.h
*
* This class will manage loading of configuration data.
*
*/
#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>
#include <String>
#include <SPIFFS.h>

class Config
{
private:
  String _configFile;
  String _subkey;
  String _pubkey;
  String _channel;

public:
  Config(String configFile)
  {
    _configFile = configFile;
  }

  // This function loads the params file data from the ESP32 file system.
  // Returns true upon successful configuration file loading and parsing.
  boolean loadParams()
  {

    // Instantiate JSON document object with 1024 bytes of room
    DynamicJsonDocument doc(1024);
    String configData;

    if (!SPIFFS.begin(true))
    {
      Serial.println("An Error has occurred while mounting SPIFFS");
      return false;
    }

    File configFile = SPIFFS.open(_configFile);
    if (!configFile)
    {
      Serial.println("Failed to open file for reading");
      return false;
    }

    char nextChar;
    Serial.print("Config file --->");
    while (configFile.available())
    {
      //Serial.write(configFile.read());
      nextChar = configFile.read();
      configData += nextChar;
    }
    Serial.println(configData);
    configFile.close();
    Serial.println();

    // Deserialize/parse the JSON document
    DeserializationError error = deserializeJson(doc, configData);

    // Test if parsing succeeds, and if so, process accordingly
    if (error)
    {
      String errorInfo = "deserializeJson() failed: ";
      errorInfo += String(error.c_str());
      Serial.println(errorInfo);
    }
    else
    {
      // Create root  JsonObject and extract values as String
      JsonObject root = doc.as<JsonObject>();
      _pubkey = root.getMember("pubkey").as<String>();
      _subkey = root.getMember("subkey").as<String>();
      _channel = root.getMember("channel").as<String>();

      Serial.println(_pubkey);
      Serial.println(_subkey);
      Serial.println(_channel);
    }

    return true;
  }

  String pubkey()
  {
    return _pubkey;
  }

  String subkey()
  {
    return _subkey;
  }

  String channel()
  {
    return _channel;
  }
};

#endif