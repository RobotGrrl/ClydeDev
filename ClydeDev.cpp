/**
 * Copyright (c) 2014 Erin Kennedy and Fabule Fabrications Inc, All rights reserved.
 * Licensed under GPL v3, see license.txt for more info.
 */

#include "ClydeDev.h"

ClydeDev::ClydeDev() {

	debug_stream = &Serial;
	LOG_LEVEL = ERROR_;

}


bool ClydeDev::init() {

	// -- pins
  pinMode(eye_r_pin, OUTPUT);
  pinMode(eye_g_pin, OUTPUT);
  pinMode(eye_b_pin, OUTPUT);
   
  pinMode(eye_ir_pin, INPUT);
   
  pinMode(white_pin, OUTPUT);
  setLight(0);


  


	if(LOG_LEVEL <= DEBUG) *debug_stream << "Beginning initialisation of Clyde" << endl; 


	current_colour[0] = 0;
	current_colour[1] = 0;
	current_colour[2] = 0;

	current_hue = 0;


  // -- eye / ir sample related
  last_calibrated = 0;
  last_sampled = 0;
  sample_count = 0;
  ir_total = 0;
  ir_range_avg = 0;
  done_calibration = false;


  // the raw, un-averaged, ir vals from analog in
  current_ir_raw = 0;
  previous_ir_raw = 0;
  ir_min_raw = 0;
  ir_max_raw = 0;


  // the averaged reading ir vals over time
  current_ir_val = 0;
  previous_ir_val = 0;


  // eye button press related
  press_thresh = 0;
  press_start = 0;
  is_pressed = false;
  press_sample_count = 0;
  last_press = 0;


	initEye();

  return true;

}


void ClydeDev::update() {

	calibrateEye();

	isEyePressed();

}



/*
 * Eye Button
 */

void ClydeDev::initEye() {

	if(LOG_LEVEL <= DEBUG) *debug_stream << "Initial calibration of eye" << endl; 
  
	bool blinky = false;

  while(!done_calibration) {
    
    calibrateEye();
    
    if(blinky) {
      setEyeHSI(hue_violet, 0.0, 1.0);
    } else {
      setEyeHSI(hue_violet, 0.0, 0.8);
    }
    
    blinky = !blinky;
    delay(100);
    
  }
  
  if(LOG_LEVEL <= DEBUG) *debug_stream << "Done calibrating eye" << endl; 

}


void ClydeDev::calibrateEye() {

  // check if it is time to calibrate or not
  if(millis()-last_sampled < TIME_THRESH) return; 
  
  
  // let's read the sensor now
  previous_ir_raw = current_ir_raw;
  current_ir_raw = analogRead(eye_ir_pin);
  
  if(LOG_LEVEL <= DEBUG) *debug_stream << "current ir raw: " << current_ir_raw << " previous ir raw: " << previous_ir_raw << endl; 
  
  
  // check if the eye is pressed -- don't calibrate if it is!
  if(isEyePressed() && press_thresh > 0) return;
  
  
  // check what the delta is- if it is not stable, we will try again next time
  uint16_t raw_delta = 0;

	if(previous_ir_raw != current_ir_raw) raw_delta = math_abs(previous_ir_raw, current_ir_raw);
  
  if(raw_delta >= DELTA_THRESH) {
    if(LOG_LEVEL <= DEBUG) *debug_stream << "too much delta in raw vals: " << raw_delta << endl; 
    return;
  }
  
  
  // capture the min and max vals
  if(sample_count == 0) {
    ir_min_raw = current_ir_raw;
    ir_max_raw = current_ir_raw;
  }
  
  if(current_ir_raw < ir_min_raw) ir_min_raw = current_ir_raw;
  if(current_ir_raw > ir_max_raw) ir_max_raw = current_ir_raw;
  
  
  // increment
  ir_total += current_ir_raw;
  sample_count++;
  
  if(LOG_LEVEL <= DEBUG) *debug_stream << "sample count: " << sample_count << endl;
  
  
  // now it is time to calculate the average val
  if(sample_count >= SAMPLE_SIZE) {
    
    // calculate average
    previous_ir_val = current_ir_val;
    float temp = (float)( (float)(ir_total) / SAMPLE_SIZE);
    current_ir_val = (uint16_t)temp;
    
    float temp2 = (float)((float)(ir_min_raw + ir_max_raw) / 2 );
    ir_range_avg = temp2;
    
    if(LOG_LEVEL <= DEBUG) *debug_stream << "current ir val: " << current_ir_val << endl; 
    if(LOG_LEVEL <= DEBUG) *debug_stream << "ir min: " << ir_min_raw << " ir max: " << ir_max_raw << endl; 
    
    
    // reset!
    ir_total = 0;
    sample_count = 0;
    
    
    // check what the range is- if it is not stable, we will try again next sample
    // commented out as we do not need it -- but we left it in case you need it
    /*
    uint16_t range_delta = math_abs(ir_min_raw, ir_max_raw);
    
    if(range_delta >= RANGE_THRESH) {
      if(LOG_LEVEL <= DEBUG) *debug_stream << "range is not stable: " << range_delta << endl; 
      return;
    }
    */
    
    
    // check what the delta is- if it is not stable, we will try again next sample
    // commented out as we do not need it -- but we left it in case you need it!
    /*
    uint16_t avg_delta = 0;

		if(current_ir_val != previous_ir_val) avg_delta = math_abs(current_ir_val, previous_ir_val);
    
    if(avg_delta >= DELTA_THRESH) {
      if(LOG_LEVEL <= DEBUG) *debug_stream << "avg delta is not stable: " << avg_delta << endl; 
      return;
    }
    */
    
    // calculate the new press theshold
    press_thresh = current_ir_val + PRESS_SENSITIVITY;
    
    
    // we are done now
    done_calibration = true;
    last_calibrated = millis();
    
  }
  
  
  last_sampled = millis();
  
}


bool ClydeDev::isEyePressed() {

  // do we detect a press, when is_pressed is not set yet?
  if(current_ir_raw > press_thresh && is_pressed == false) {
    
    
    if(LOG_LEVEL <= DEBUG) *debug_stream << "detected press " << press_sample_count << endl;


    // reset the counter if this is the first press in quite some time
    if(millis()-last_press >= PRESS_LAST_RESET && last_press != 0) {
      press_sample_count = 0;
    }
    
    
    // increment and record the press...
    press_sample_count++;
    last_press = millis();
    
    
    // if we exceed the specific number of presses, then set is_pressed!
    if(press_sample_count > PRESS_COUNT_THRESH) {
      
      press_start = millis();
      is_pressed = true;
      press_sample_count = 0;

      if(LOG_LEVEL <= DEBUG) *debug_stream << "pressed1!" << endl;

    }
    
  }
  
  
  // do we detect a press, when is_pressed is set?
  if(current_ir_raw > press_thresh && is_pressed == true) {
    
    if(LOG_LEVEL <= DEBUG) *debug_stream << "pressed! " << current_ir_raw << " > " << press_thresh << endl;
    
    last_press = millis();

    if(eyePressedHandler) eyePressedHandler();
    
  }
  

  // is it pressed? we should check if it is still pressed
  if(is_pressed == true) {
   

    // check that the press is not timed out yet -- otherwise, return true
    // alternatively, if you want the press to time out after a certain
    // amount of time, you can do something like this:
    //if(millis()-press_start >= PRESS_START_TIMEOUT 
    if(millis()-last_press >= PRESS_LAST_TIMEOUT) {
      if(LOG_LEVEL <= DEBUG) *debug_stream << "press time's up" << endl;
      is_pressed = false;
      last_press = 0;

      if(eyeReleasedHandler) eyeReleasedHandler();

      return false;
    }
    
    return true;
    
  }
  
  return false;  
  
}


uint16_t ClydeDev::math_abs(uint16_t a, uint16_t b) {
  
  // for some reason the abs() function is giving us a negative number,
  // even though we are not passing a function to the macro. weird behaviour.
  // it is seemingly because it is not ordering the numbers properly.
  
  if(a > b) return abs(a-b);
  
  if(b > a) return abs(b-a);
  
}



/*
 * Eye LED Functions
 */

void ClydeDev::setEyeRGB(uint8_t r, uint8_t g, uint8_t b) {
  
	double hsv[3];

	rgb2hsv(r, g, b, hsv);

  current_colour[0] = r;
  current_colour[1] = g;
  current_colour[2] = b;
  current_hue = (float)hsv[0];
  
  analogWrite(eye_r_pin, r * SCALE_CONSTRAINT);
  analogWrite(eye_g_pin, g * SCALE_CONSTRAINT);
  analogWrite(eye_b_pin, b * SCALE_CONSTRAINT);
  
}


void ClydeDev::setEyeHSI(float H, float S, float I) {
  
  int rgb[3];
  
  hsi2rgb(H, S, I, rgb);
  
  
  uint8_t new_r = (uint8_t)(rgb[0] * SCALE_CONSTRAINT); 
  uint8_t new_g = (uint8_t)(rgb[1] * SCALE_CONSTRAINT);
  uint8_t new_b = (uint8_t)(rgb[2] * SCALE_CONSTRAINT);
  
  current_colour[0] = new_r;
  current_colour[1] = new_g;
  current_colour[2] = new_b;
  current_hue = H;
  
  analogWrite(eye_r_pin, new_r);
  analogWrite(eye_g_pin, new_g);
  analogWrite(eye_b_pin, new_b);
  
}


// This function is by Brian Neltner from Saikoled
// http://blog.saikoled.com/post/43693602826/why-every-led-light-should-be-using-hsi-colorspace
void ClydeDev::hsi2rgb(float H, float S, float I, int *rgb) {
  
  int r, g, b;
  H = fmod(H,360); // cycle H around to 0-360 degrees
  H = 3.14159*H/(float)180; // Convert to radians.
  S = S>0?(S<1?S:1):0; // clamp S and I to interval [0,1]
  I = I>0?(I<1?I:1):0;
    
  // Math! Thanks in part to Kyle Miller.
  if(H < 2.09439) {
    r = 255*I/3*(1+S*cos(H)/cos(1.047196667-H));
    g = 255*I/3*(1+S*(1-cos(H)/cos(1.047196667-H)));
    b = 255*I/3*(1-S);
  } else if(H < 4.188787) {
    H = H - 2.09439;
    g = 255*I/3*(1+S*cos(H)/cos(1.047196667-H));
    b = 255*I/3*(1+S*(1-cos(H)/cos(1.047196667-H)));
    r = 255*I/3*(1-S);
  } else {
    H = H - 4.188787;
    b = 255*I/3*(1+S*cos(H)/cos(1.047196667-H));
    r = 255*I/3*(1+S*(1-cos(H)/cos(1.047196667-H)));
    g = 255*I/3*(1-S);
  }
  
  rgb[0]=r;
  rgb[1]=g;
  rgb[2]=b;
  
}


// This function is by Robert Atkins
// https://github.com/ratkins/RGBConverter
void ClydeDev::rgb2hsv(uint8_t r, uint8_t g, uint8_t b, double *hsv) {
    double rd = (double) r/255;
    double gd = (double) g/255;
    double bd = (double) b/255;
    double max = threeway_max(rd, gd, bd), min = threeway_min(rd, gd, bd);
    double h, s, v = max;

    double d = max - min;
    s = max == 0 ? 0 : d / max;

    if (max == min) { 
        h = 0; // achromatic
    } else {
        if (max == rd) {
            h = (gd - bd) / d + (gd < bd ? 6 : 0);
        } else if (max == gd) {
            h = (bd - rd) / d + 2;
        } else if (max == bd) {
            h = (rd - gd) / d + 4;
        }
        h /= 6;
    }

    hsv[0] = h;
    hsv[1] = s;
    hsv[2] = v;
}

double ClydeDev::threeway_max(double a, double b, double c) {
    return max(a, max(b, c));
}

double ClydeDev::threeway_min(double a, double b, double c) {
    return min(a, min(b, c));
}


/*
 * White Light Functions
 */
 
void ClydeDev::setLight(uint8_t b) {
  
  analogWrite(white_pin, 255-b);
  
}





