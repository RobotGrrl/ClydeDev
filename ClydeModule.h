/**
 * Copyright (c) 2014 Erin Kennedy and Fabule Fabrications Inc, All rights reserved.
 * Licensed under GPL v3, see license.txt for more info.
 */

#ifndef __CLYDEMODULE_H
#define __CLYDEMODULE_H

#include <stdint.h>

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif


class ClydeModule {

private:
  const uint16_t ID_LOW;  // low limit of read identification value
  const uint16_t ID_HIGH; // high limit of read identification value

  uint8_t m_apin;
  uint8_t m_dpin;
  
public:

  static const uint8_t ID_REPEAT = 8;

  
  ClydeModule(uint16_t low, uint16_t high) : ID_LOW(low), ID_HIGH(high), m_apin(0), m_dpin(0) {};
  
  virtual bool init() = 0;
  virtual void update() = 0;


  bool id(uint16_t value) { return value >= ID_LOW && value <= ID_HIGH; }

  uint8_t apin() { return m_apin; }
  uint8_t dpin() { return m_dpin; }


  // if you eventually ever need to add more modules to clyde, this would be
  // the place to add their respective a and d pin assignments
  void determineModulePins(uint8_t m) {

    if(m == 1) {
      m_dpin = 7;
      m_apin = 1;
    } else if(m == 2) {
      m_dpin = 8;
      m_apin = 2;
    }

  }


  // this finds the analog id value, we have to turn pins on and off in 
  // a special order apparently.
  uint16_t idValue() {

    pinMode(m_dpin, OUTPUT);
    digitalWrite(m_dpin, HIGH);
    uint16_t idValue = analogRead(m_apin);
    pinMode(m_dpin, INPUT);
    delay(10);

    return idValue;

  }


  // count if the number of times the resistor id hits within the id range,
  // if it is >= 5 (arbitrary number), then chances are it is indeed the
  // correct resistor.
  bool checkModuleId() {

    uint8_t id_counter = 0;

    for(int i=0; i<ID_REPEAT; i++) {
    
      uint16_t idVal = idValue();

      //Serial.print("id val: ");
      //Serial.println(idVal);

      if( id( idVal ) ) {
        id_counter++;
      }
      
      //Serial.println(id_counter);

      if(id_counter >= 5) {
        return true;
        break;
      }
    
    }

    return false;

  }



};

#endif
