/**
 * Copyright (c) 2014 Erin Kennedy and Fabule Fabrications Inc, All rights reserved.
 * Licensed under GPL v3, see license.txt for more info.
 */

#include "ClydeTouchyFeely.h"
#include "I2Cdev.h"

ClydeTouchyFeely::ClydeTouchyFeely(uint8_t m)
 : ClydeModule(ID_LOW, ID_HIGH), m_mpr121(DEVICE_ADDR, TOUCH_LEVEL, RELEASE_LEVEL) {


	debug_stream = &Serial;
	LOG_LEVEL = ERROR_;


	ClydeModule::determineModulePins(m);

	apin = ClydeModule::apin();
	dpin = ClydeModule::dpin();

	for(int i=0; i<NUM_LEGS; i++) {
		leg_touched[i] = false;
		leg_start_touch[i] = 0;
	}

}


bool ClydeTouchyFeely::init() {

	if(LOG_LEVEL <= DEBUG) *debug_stream << "Beginning Touchy Feely module initialisation" << endl;

	Wire.begin();

	// check the id of the module to see that it matches
	bool passed = ClydeModule::checkModuleId();

  if(!passed) {
   	if(LOG_LEVEL <= WARN) *debug_stream << "Error initialising the Touchy Feely module. Failed to read ID value." << endl;
		return false;
	}


	if (!m_mpr121.testConnection()) {
    if(LOG_LEVEL <= WARN) *debug_stream << "Failed to initialize Touchy Feely module. Failed to connect to MPR121" << endl;
    return false;
  }

  m_mpr121.initialize(false);

  pinMode(dpin, INPUT);
  digitalWrite(dpin, LOW);

  if(LOG_LEVEL <= DEBUG) *debug_stream << "Touchy Feely module initialised!" << endl;

  return true;

}


void ClydeTouchyFeely::update() {

	// let's update the status of all the legs
	for(int i=0; i<NUM_LEGS; i++) {

		bool stat = m_mpr121.getTouchStatus(i);


		// if this is a new touch that we have detected...
		// wait for a bit before classifying it as a touch
		if(stat == true && leg_touched[i] == false) {

			if(leg_start_touch[i] == 0) {
				leg_start_touch[i] = millis();
				if(LOG_LEVEL <= DEBUG) *debug_stream << "Detected: " << i << endl;
				if(detectedHandler) detectedHandler(i);
			}

			if(millis()-leg_start_touch[i] >= LEG_START_THRESH) {
				leg_touched[i] = true;
			}

		}


		// if a touch is there, send it to the callback method
		if(stat == true && leg_touched[i] == true) {
			if(LOG_LEVEL <= DEBUG) *debug_stream << "Touch: " << i << endl;
			if(touchedHandler) touchedHandler(i);
		}


		// check to see if any touches have been released
		if(stat == false && leg_touched[i] == true) {

			if(millis()-leg_start_touch[i] >= LEG_STOP_THRESH) {
				leg_touched[i] = false;
				leg_start_touch[i] = 0;
				if(LOG_LEVEL <= DEBUG) *debug_stream << "Released: " << i << endl;
				if(releasedHandler) releasedHandler(i);
			}

		}



	}

}

// that's just calling the reset on the mp3121.
void ClydeTouchyFeely::reset(bool autoconfig=false, uint8_t touch=0, uint8_t release=0){
  m_mpr121.reset( autoconfig, touch, release );
}
