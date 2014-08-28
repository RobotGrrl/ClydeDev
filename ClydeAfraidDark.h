/**
 * Copyright (c) 2014 Erin Kennedy and Fabule Fabrications Inc, All rights reserved.
 * Licensed under GPL v3, see license.txt for more info.
 */

#ifndef _CLYDEAFRAIDDARK_H_
#define _CLYDEAFRAIDDARK_H_

#include "ClydeModule.h"

#include <Streaming.h>

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


class ClydeAfraidDark : public ClydeModule {

  private:

    // -- resistor id vals
    static const uint16_t ID_LOW = 900;            // low limit of read identification value
    static const uint16_t ID_HIGH = 920;           // high limit of read identification value

    // -- module pins
    uint8_t apin;
    uint8_t dpin;

    // -- misc
    Stream *debug_stream;

    enum Level {
      DEBUG,
      INFO,
      WARN,
      ERROR_
    };

    Level LOG_LEVEL;

    // -- light
    uint16_t light;

  public:

    ClydeAfraidDark(uint8_t m);

    bool init();
    void update();

    uint16_t getLight();

    void setDebugStream(Stream *d, int l) { debug_stream = d; LOG_LEVEL = (Level)l; }

};

#endif
