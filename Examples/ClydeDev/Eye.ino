void clydeEyePressed() {
 
 Serial << "Callback- Eye Pressed!" << endl;
 
 if(!currently_pressed) {
   start_r = clyde.current_colour[0];
   start_g = clyde.current_colour[1];
   start_b = clyde.current_colour[2];
   currently_pressed = true;
 }
 
 
 if(millis()-last_blink >= 100) {
   
   uint8_t r = 0;
   uint8_t g = 0;
   uint8_t b = 0;
   
   if(blink_on) {
     
     r = 0;
     g = 0;
     b = 0;
     
     if(clyde.current_colour[0] > 20) r = clyde.current_colour[0]-20;
     if(clyde.current_colour[1] > 20) g = clyde.current_colour[1]-20;
     if(clyde.current_colour[2] > 20) b = clyde.current_colour[2]-20;
     
   } else {
     
     r = 255;
     g = 255;
     b = 255;
     
     if(clyde.current_colour[0] < 255) r = clyde.current_colour[0]+20;
     if(clyde.current_colour[1] < 255) g = clyde.current_colour[1]+20;
     if(clyde.current_colour[2] < 255) b = clyde.current_colour[2]+20;
     
   }
   
   clyde.setEyeRGB(r, g, b);
   
   blink_on = !blink_on;
   
   last_blink = millis();
   
 }
  
}

void clydeEyeReleased() {
 
  Serial << "Callback- Eye Released!" << endl;
 
  clyde.setEyeRGB(start_r, start_g, start_b);
  
  currently_pressed = false;
  
}


