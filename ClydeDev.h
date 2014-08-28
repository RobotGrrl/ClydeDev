/**
 * Clyde Dev Library
 * -----------------
 * 
 * By Erin 'RobotGrrl' Kennedy
 * --> http://robotgrrl.com
 *
 * Based on the code by Fabule Fabrications Inc!
 * --> http://fabule.com
 * 
 * Copyright (c) 2014 Erin Kennedy and Fabule Fabrications Inc, All rights reserved.
 * Licensed under GPL v3, see license.txt for more info.
 *
 */

#ifndef _CLYDEDEV_H_
#define _CLYDEDEV_H_

#include <Wire.h>
#include <Streaming.h>


#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


class ClydeDev {

  private:

    // -- pins
    static const uint8_t eye_r_pin = 5;
    static const uint8_t eye_g_pin = 6;
    static const uint8_t eye_b_pin = 9;

    static const uint8_t eye_ir_pin = A0;

    static const uint8_t white_pin = 11;


    // -- eye
    
    // this is so the brightness of the leds do not interfere with the ir sensor
    static const float SCALE_CONSTRAINT = 0.50;//0.88; 


    // -- eye button

     // sample every x milliseconds
    static const uint16_t TIME_THRESH = 150;

    // the size of our sample window, used to determine the
    // current_ir_val and press_thresh for the eye button
    static const uint16_t SAMPLE_SIZE = 10;

    // the threshold for ignoring raw data from the
    // difference between current and previous value
    static const uint16_t DELTA_THRESH = 80;

    // the threshold for throwing out data based on the
    // difference between the ir max and min
    static const uint16_t RANGE_THRESH = 40;

    // the number of presses to count to for is_pressed 
    // to be set to true
    static const uint8_t PRESS_COUNT_THRESH = 6;

    // larger: less sensitive, smaller: more sensitive
    static const uint16_t PRESS_SENSITIVITY = 300;

    // the amount of time (ms) from the last press to reset the
    // press count to 0 -- specifically when is_pressed is false
    static const uint16_t PRESS_LAST_RESET = 250;

    // if the amount of time from the start of the press to
    // now exceeds this amount, the press will be released
    static const uint16_t PRESS_START_TIMEOUT = 3000;

    // if the amount of time from the last press to now exceeds
    // this amount, then the press will be released
    static const uint16_t PRESS_LAST_TIMEOUT = 250;


    // -- eye / ir sample related
    long last_calibrated;
    long last_sampled;
    uint16_t sample_count;
    uint16_t ir_total;
    uint16_t ir_range_avg;
    bool done_calibration;


    // the raw, un-averaged, ir vals from analog in
    uint16_t current_ir_raw;
    uint16_t previous_ir_raw;
    uint16_t ir_min_raw;
    uint16_t ir_max_raw;


    // the averaged reading ir vals over time
    uint16_t current_ir_val;
    uint16_t previous_ir_val;


    // eye button press related
    uint16_t press_thresh;
    long press_start;
    bool is_pressed;
    uint8_t press_sample_count;
    long last_press;


    // -- misc
  	Stream *debug_stream;


    // -- eye button
    void initEye();
    void calibrateEye();
    uint16_t math_abs(uint16_t a, uint16_t b);


    // -- eye led
    double threeway_max(double a, double b, double c);
    double threeway_min(double a, double b, double c);


    // -- callback methods
    void (*eyePressedHandler)();
    void (*eyeReleasedHandler)();


  public:

    // -- clyde
  	ClydeDev();
    bool init();
  	void update();


    // -- white light
    void setLight(uint8_t b);


    // -- eye led
    uint8_t current_colour[3];
    float current_hue;
    
    void setEyeRGB(uint8_t r, uint8_t g, uint8_t b);
    void setEyeHSI(float H, float S, float I);
    void hsi2rgb(float H, float S, float I, int *rgb);
    void rgb2hsv(uint8_t r, uint8_t g, uint8_t b, double *hsv);


    // -- eye button
    bool isEyePressed();


    // -- callback methods
    void setEyePressedHandler(void(*function)()) { eyePressedHandler = function; }
    void setEyeReleasedHandler(void(*function)()) { eyeReleasedHandler = function; }
    

    // -- misc
    enum Level {
      DEBUG,
      INFO,
      WARN,
      ERROR_
    };

    Level LOG_LEVEL;

    void setDebugStream(Stream *d, int l) { debug_stream = d; LOG_LEVEL = (Level)l; }


    // -- common colours
    static const float hue_red = 330.0;
    static const float hue_orange = 20.0;
    static const float hue_yellow = 45.0;
    static const float hue_green = 90.0;
    static const float hue_blue = 210.0;
    static const float hue_violet = 270.0;

};

#endif
