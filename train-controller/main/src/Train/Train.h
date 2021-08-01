/* Train.h
*
* MCU train controller object for basic controls like
* forward/reverse, speed, lights on/off/blink, and possibly
* additional functionality in the future.
*
*/
#ifndef TRAIN_H
#define TRAIN_H

#include <String>
#include <stdint.h>
#include "SCMD.h"
#include "SCMD_config.h" //Contains #defines for common SCMD register names and values
#include "Wire.h"


class Train {
  /* Private object properties and state */
  private:
    SCMD motorDriver; //This creates the main object of one motor driver and connected peripherals.
    String _name = "";
    int _speed = 0;
    String _direction = "forward"; // or "reverse"
    int _headlight_left_pin = 0;
    int _headlight_right_pin = 0;

    /* Public Methods */
  public:

    String name() {
      return _name;
    }

    Train(String name, int headlight_left_pin, int headlight_right_pin) {
      _name = name;
      _headlight_left_pin = headlight_left_pin;
      _headlight_right_pin = headlight_right_pin;

      // Set up pin mode to write for headlight LEDs.
      pinMode(_headlight_left_pin, OUTPUT);
      pinMode(_headlight_right_pin, OUTPUT);
      pinMode(LED_BUILTIN, OUTPUT);
    }

    void begin() {
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
      Serial.print("Waiting for enumeration...");
      while (motorDriver.ready() == false);
      Serial.println("Done.");
      Serial.println();

      //  ***** wait for driver to become ready, and enable ***** //
      Serial.println("Waiting for motor driver to become ready...");
      while (motorDriver.busy());
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

    void ready() {
      // Turn on built-in LED when instantiated to confirm ready status
      digitalWrite(LED_BUILTIN, HIGH);
    }

    void setDirection(String direction) {
      if (direction == "forward") {

      }
    }

    void fullSteamAhead() {
      Serial.println("Full steam ahead!!");
      // pass setDrive() a motor number, direction as 0(call 0 forward) or 1, and level from 0 to 255
      motorDriver.setDrive( 0, 0, 0); // Stop motor

      // Smoothly move motor up to speed and back (drive level 0 to 255)
      for (int i = 0; i < 256; i++)
      {
        motorDriver.setDrive( 0, 0, i);
        delay(5);
      }
      delay(1000);
      for (int i = 255; i >= 0; i--)
      {
        motorDriver.setDrive( 0, 0, i);
        delay(5);
      }

    }

    void headlightsOn() {
      digitalWrite(_headlight_left_pin, HIGH);
      digitalWrite(_headlight_right_pin, HIGH);
    }

    void headlightsOff() {
      digitalWrite(_headlight_left_pin, LOW);
      digitalWrite(_headlight_right_pin, LOW);
    }

    // TODO: Finish this, implement PWM to pulse lights back and forth
    // void pulseLights() {
    //   Serial.println("alternating lights");
    //   int 
    // }
};

#endif