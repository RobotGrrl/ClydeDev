void clydeTouched(uint8_t l) {
 
  Serial << "Callback- Touched! " << l << endl;
  
  switch(l) {
    case 0:
      // cycle red
      cycleRGB(0);
    break;
    case 1:
      // cycle green
      cycleRGB(1);
    break;
    case 2:
      // cycle blue
      cycleRGB(2);
    break;
    case 3:
      // cycle the light
      cycleLight();
    break;
    case 4:
      // no connection to this leg
    break;
    case 5:
      // go through all the colours
      cycleHue();
    break;
  }
  
}

void clydeReleased(uint8_t l) {
 
  Serial << "Callback- Released!" << l << endl;
  
  // change all the directions -- maybe just to confuse people ;)
  
  switch(l) {
    case 0:
      // cycle red
      red_dir = !red_dir;
    break;
    case 1:
      // cycle green
      green_dir = !green_dir;
    break;
    case 2:
      // cycle blue
      blue_dir = !blue_dir;
    break;
    case 3:
      // cycle the light
      light_dir = !light_dir;
    break;
    case 4:
      // no connection to this leg
    break;
    case 5:
      hue_dir = !hue_dir;
    break;
  }
  
}

void clydeDetected(uint8_t l) {

  Serial << "Callback- Detected!" << l << endl;
  
}

