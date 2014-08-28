/**
 * Copyright (c) 2014 Erin Kennedy and Fabule Fabrications Inc, All rights reserved.
 * Licensed under GPL v3, see license.txt for more info.
 */

#include "ClydeAfraidDark.h"

ClydeAfraidDark::ClydeAfraidDark(uint8_t m) : ClydeModule(ID_LOW, ID_HIGH) {

	debug_stream = &Serial;
	LOG_LEVEL = ERROR_;
	

	ClydeModule::determineModulePins(m);

	apin = ClydeModule::apin();
	dpin = ClydeModule::dpin();

	light = 0;

}


bool ClydeAfraidDark::init() {

	if(LOG_LEVEL <= DEBUG) *debug_stream << "Beginning Afraid Dark module initialisation" << endl; 

	// check the id of the module to see that it matches
	bool passed = ClydeModule::checkModuleId();

  if(!passed) {
   	if(LOG_LEVEL <= WARN) *debug_stream << "Error initialising the Afraid Dark module. Failed to read ID value." << endl;
		return false;
	}

  pinMode(dpin, INPUT);
  digitalWrite(dpin, LOW);
  
  if(LOG_LEVEL <= DEBUG) *debug_stream << "Afraid Dark module initialised!" << endl; 
  
  return true;

}


void ClydeAfraidDark::update() {
	getLight();
}


uint16_t ClydeAfraidDark::getLight() {
	light = analogRead(apin);
	return light;
}
