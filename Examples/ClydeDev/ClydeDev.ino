/*
 * Clyde Dev Example
 * -----------------
 *
 * Example sketch for the Clyde Dev library. Go wild!
 *
 * In this demo, you can touch one of Clyde's legs to change
 * the white light or the RGB light. For example, one leg
 * will change the hue.
 *
 * Each time you release the touch, then touch again, the
 * 'direction' is reversed. (Light->Dark, Dark->Light).
 *
 * Pressing the eye button will make it blink.
 *
 * Try to mix colours by touching some of the legs to add in
 * R, G, B, values. It is quite fun.
 *
 * Library requirements:
 * - ClydeDev
 * - Streaming
 * - MPR121 & I2Cdev
 *
 * For more and up-to-date info see the Github repository:
 * --> https://github.com/RobotGrrl/ClydeDev
 * --> https://github.com/fabule
 *
 * For more info about Clyde:
 * --> http://fabule.com
 *
 * By Erin RobotGrrl
 * --> http://robotgrrl.com
 *
 * Based on the original Clyde code by Fabule Fabrications Inc.
 *
 * August 27, 2014
 *
 */

#include <Wire.h>
#include <MPR121.h>
#include <Streaming.h>

#include <ClydeDev.h>
#include <ClydeTouchyFeely.h>
#include <ClydeAfraidDark.h>


// -- clyde dev
ClydeDev clyde = ClydeDev();


// -- touchy feely module in module port 2
ClydeTouchyFeely touchyfeely = ClydeTouchyFeely(2);
boolean tf_enabled = false;


// afraid of the dark module in module port 1
ClydeAfraidDark afraiddark = ClydeAfraidDark(1);
boolean ad_enabled = false;


// -- behaviour
long last_blink = 0;
boolean blink_on = false;
float hue_num = ClydeDev::hue_green;
int light_num = 10;
boolean hue_dir = true;
boolean light_dir = true;
boolean red_dir = true;
boolean green_dir = true;
boolean blue_dir = true;
float intensity = 0.3;
boolean currently_pressed = false;
uint8_t start_r = 0;
uint8_t start_g = 0;
uint8_t start_b = 0;


void setup() {
  
  Serial.begin(9600);
  while(!Serial);
  
  delay(5000);
  
  // -- clyde!
  //clyde.setDebugStream(&Serial, ClydeDev::DEBUG); // uncomment if you want to see debug text
  clyde.init();
  
  clyde.setEyePressedHandler(clydeEyePressed);
  clyde.setEyeReleasedHandler(clydeEyeReleased);
  
  
  // -- touchy feely module
  //touchyfeely.setDebugStream(&Serial, ClydeDev::DEBUG); // uncomment if you want to see debug text
  tf_enabled = touchyfeely.init();
  
  if(tf_enabled) {
    touchyfeely.setTouchedHandler(clydeTouched);
    touchyfeely.setReleasedHandler(clydeReleased);
    touchyfeely.setDetectedHandler(clydeDetected);
  }
  
  
  // -- afraid of the dark module
  //afraiddark.setDebugStream(&Serial, ClydeDev::DEBUG); // uncomment if you want to see debug text
  ad_enabled = afraiddark.init();
  
  Serial << "Hello! :3" << endl;
  
  clyde.setEyeHSI(hue_num, 1.0, intensity);
  clyde.setLight(light_num);
  
}

void loop() {
  
  clyde.update();
  
  if(tf_enabled) touchyfeely.update();
  
  if(ad_enabled) {
    afraiddark.update();
    
    // uncomment this if you want the light sensor to control the intensity
    // of the rgb light... the result is a little glitchy though, as it is
    // using the raw sensor values.
    /*
    uint16_t light_level = afraiddark.getLight();
    intensity = ((float)light_level)/1000.0;
    
    Serial << "light level: " << light_level << " intensity: " << intensity << endl;
    */
    
  }
  
}


