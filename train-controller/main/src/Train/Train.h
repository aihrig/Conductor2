/* Train.h
*
* MCU train controller object for basic controls like
* forward/reverse, speed, lights on/off/blink, and possibly
* additional functionality in the future.
*
*/
#ifndef TRAIN_H
#define TRAIN_H
#include <ArduinoJson.h>
#include <String>
#include <stdint.h>
#include "SCMD.h"
#include "SCMD_config.h" //Contains #defines for common SCMD register names and values
#include "Wire.h"

class Train
{
  /* Private object properties and state */
private:
  SCMD motorDriver; //This creates the main object of one motor driver and connected peripherals.
  String _name = "";
  int _speed = 0; // Speed from 0 - 100 for human purposes
  bool _headlightsOn = false;
  String _direction = "forward"; // or "reverse"
  int _headlight_left_pin = 0;
  int _headlight_right_pin = 0;
  const int FORWARD = 0;
  const int REVERSE = 1;


  /* Public Methods */
public:
  String name()
  {
    return _name;
  }

  Train(String name, int headlight_left_pin, int headlight_right_pin)
  {
    _name = name;
    _headlight_left_pin = headlight_left_pin;
    _headlight_right_pin = headlight_right_pin;

    // Set up pin mode to write for headlight LEDs.
    pinMode(_headlight_left_pin, OUTPUT);
    pinMode(_headlight_right_pin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

  }

  void begin()
  {
    //***** Configure the Motor Driver's Settings *****//
    // commInterface is I2C_MODE
    motorDriver.settings.commInterface = I2C_MODE;

    // set address if I2C configuration selected with the config jumpers
    motorDriver.settings.I2CAddress = 0x5D; //config pattern is "1000" (default) on board for address 0x5D

    // set chip select if SPI selected with the config jumpers
    motorDriver.settings.chipSelectPin = 10;

    //*****initialize the driver get wait for idle*****//
    while (motorDriver.begin() != 0xA9) //Wait until a valid ID word is returned
    {
      Serial.println("ID mismatch, trying again");
      delay(500);
    }
    Serial.println("ID matches 0xA9");

    // Check to make sure the driver is done looking for peripherals before beginning
    Serial.print("Waiting for peripheral enumeration...");
    while (motorDriver.ready() == false)
      ;
    Serial.println("Done.");
    Serial.println();

    //  ***** wait for driver to become ready, and enable ***** //
    Serial.println("Waiting for motor driver to become ready...");
    while (motorDriver.busy())
      ;
    motorDriver.enable(); //Enables the output driver hardware
    Serial.println("Done.");
    Serial.println();

    // Signal that train is ready
    this->headlightsOn();
    delay(500);
    this->headlightsOff();
    delay(500);
    this->headlightsOn();
    delay(500);
    this->headlightsOff();
  }

  void ready()
  {
    // TODO: Move this out of Train class
    // Turn on built-in LED when instantiated to confirm ready status
    digitalWrite(LED_BUILTIN, HIGH);
  }

  // DEBUG: For testing motors
  void fullSteamAhead()
  {
    Serial.println("Full steam ahead!!");
    // pass setDrive() a motor number, direction as 0(call 0 forward) or 1, and level from 0 to 255
    motorDriver.setDrive(0, 0, 0); // Stop motor

    // Smoothly move motor up to speed and back (drive level 0 to 255)
    for (int i = 0; i < 256; i++)
    {
      motorDriver.setDrive(0, 0, i);
      delay(5);
    }
    delay(250);
    for (int i = 255; i >= 0; i--)
    {
      motorDriver.setDrive(0, 0, i);
      delay(5);
    }
  }

  // TODO: Find or build async delay library to ramp up/down speed
  void driveRamp(String direction, int speedPct)
  {
    int dir = 0;

    Serial.print("drive(): ");
    Serial.print(direction + " ");
    Serial.println(speedPct);

    if (direction == "reverse")
    {
      dir = 1;
    }

    // Only accept valid percentage of speed & convert to motor driver range 0 - 255
    if (speedPct >= 0 && speedPct <= 100) {
      int speed = 0;
      if (speedPct == 0)
      {
        speed = 0;
      }
      else{
        speed = map(speedPct, 0, 100, 50, 255);
      }
      Serial.println("Mapped speed: " + String(speed));
      // pass setDrive() a motor number, direction 0 (forward) or 1 (reverse), and level from 0 to 255
      motorDriver.setDrive(0, dir, speed);
      _speed = speedPct;
    }
  }

  void stop()
  {
    // pass setDrive() a motor number, direction 0 (forward) or 1 (reverse), and level from 0 to 255
    motorDriver.setDrive(0, 0, 0); // Stop motor
    _speed = 0;
  }

  // TODO: Create a toggleHeadlights() function
  void headlightsOn()
  {
    digitalWrite(_headlight_left_pin, HIGH);
    digitalWrite(_headlight_right_pin, HIGH);
    _headlightsOn = true;
  }

  void headlightsOff()
  {
    digitalWrite(_headlight_left_pin, LOW);
    digitalWrite(_headlight_right_pin, LOW);
    _headlightsOn = false;
  }

  // TODO: Finish this, implement PWM to pulse lights back and forth
  // void pulseLights() {
  //   Serial.println("alternating lights");
  //   int
  // }

  // TODO: Add a getHumanStatus??
  String getStatus() {
    String statusMessage;
    statusMessage = "{\"status\": {\"speed\":\"" + String(_speed) + "\", \"headlightsOn\":\"" + String(_headlightsOn) + "\"} }";
    return statusMessage;
  }

  void handleCommandMessages(const DynamicJsonDocument &doc)
  {
    // serializeJson(doc, Serial); // DEBUG: write doc to serial
    String command = doc[0]["command"];
    // Serial.println(command);
    // Serial.println(String("Train.h: " + command));

    /* Drive commands 
         example: [{"command" : "drive", "direction" : "forward", "speed" : "25"}]
      */
    if (command == "drive")
    {
      Serial.println("Command: drive ");
      // serializeJson(doc, Serial); // DEBUG: write doc to serial
      driveRamp(doc[0]["direction"], doc[0]["speed"]);
    }

    // TODO: Change this to use 1/0 instead of on/off
    /* Headlight commands 
         example: [{"command":"headlights","data":"on"}]
      */
    if (command == "headlights")
    {
      const String data = doc[0]["data"];
      Serial.print("Turning headlights");
      if (data == "on")
      {
        Serial.println(" on.");
        this->headlightsOn();
      }
      if (data == "off")
      {
        Serial.println(" off.");
        this->headlightsOff();
      }
    }

    if (command == "fullsteamahead")
    {
      Serial.println("Full steam ahead!");
      this->fullSteamAhead();
    }
  }
};

#endif