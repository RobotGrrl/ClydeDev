/**
 * Copyright (c) 2014 Erin Kennedy and Fabule Fabrications Inc, All rights reserved.
 * Licensed under GPL v3, see license.txt for more info.
 */

#ifndef _CLYDETOUCHYFEELY_H_
#define _CLYDETOUCHYFEELY_H_

#include "ClydeModule.h"

#include <Wire.h>
#include <MPR121.h>
#include <Streaming.h>

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


class ClydeTouchyFeely : public ClydeModule {

  private:

    // -- resistor id vals
  	static const uint16_t ID_LOW = 1000;            // low limit of read identification value
	  static const uint16_t ID_HIGH = 1010;           // high limit of read identification value

    // -- mpr121 related
    static const uint8_t DEVICE_ADDR = 0x5A;        // i2c address of mpr121
    static const uint8_t TOUCH_LEVEL = 0x06;        // touch threshold of mpr121
    static const uint8_t RELEASE_LEVEL = 0x04;      // release threshold of mpr121
    MPR121 m_mpr121;

    // -- legs

    // delay from first detected touch to classifying as a touch
    static const uint8_t LEG_START_THRESH = 100;

    // delay from the last detected touch to the detected touch time, for classifying as released
    static const uint8_t LEG_STOP_THRESH = 1000;

    // just in case there is one day a mega-clyde with 5 billion legs
    static const uint8_t NUM_LEGS = 6;

    bool leg_touched[NUM_LEGS];
    long leg_start_touch[NUM_LEGS];

    // -- module pins
    uint8_t apin;
    uint8_t dpin;

    // -- callback methods
	  void (*touchedHandler)(uint8_t l);
  	void (*releasedHandler)(uint8_t l);
  	void (*detectedHandler)(uint8_t l);

    // -- misc
  	Stream *debug_stream;

    enum Level {
      DEBUG,
      INFO,
      WARN,
      ERROR_
    };

    Level LOG_LEVEL;

  public:

  	ClydeTouchyFeely(uint8_t m);

  	bool init();
  	void update();
	void reset( bool autoconfig=false, uint8_t touch=0, uint8_t release=0 );

    void setTouchedHandler(void(*function)(uint8_t l)) { touchedHandler = function; }
  	void setReleasedHandler(void(*function)(uint8_t l)) { releasedHandler = function; }
    void setDetectedHandler(void(*function)(uint8_t l)) { detectedHandler = function; }

    void setDebugStream(Stream *d, int l) { debug_stream = d; LOG_LEVEL = (Level)l; }

};

#endif
