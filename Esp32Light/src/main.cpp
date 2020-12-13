#include <Arduino.h>
#include <FastLED.h>
#include <BS_LightToSerial.h>
#include "main.hpp"

// global vars
CRGB rightColor;
CRGB leftColor;
bool IsChroma = false;
//CRGB leds[NUM_LEDS];
CRGBArray<NUM_LEDS> leds;
//CRGBSet partA(leds(6,9));


struct Lights stripeControl[7];
Laser leftLaser;
Laser rightLaser;
BS_LightToSerial bslts;


void setup() {
  rightColor.setRGB(0,125,255);
  leftColor.setRGB(255,0,0);
  delay(100);
  //setup serial
  Serial.begin(250000);
  int valuesMinMax[] = {LEDSTART, LEDSPLIT1, LEDSPLIT2, LEDSPLIT3, LEDSPLIT4, LEDSPLIT5, LEDSPLIT6, LEDEND};
  for (size_t i = 0; i < 7; i++)
  {
    stripeControl[i].MIN = valuesMinMax[i];
    stripeControl[i].MAX = valuesMinMax[i+1];
    //stripeControl[i].lightcontroller = CRGBSet(leds(valuesMinMax[i], valuesMinMax[i+1]-1));
  }
  FastLED.addLeds<LEDTYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  //FastLED.setMaxRefreshRate(1200);
  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  leftLaser.index = 1;
  rightLaser.index = 5;
  
  delay(100);
}


CRGB col;

void loop() {
  /* */
//Fade in progress?
EVERY_N_MILLISECONDS(6) {
  for (int i = 0; i < 7; i++)
  {
    if(stripeControl[i].status.FADE == 1) {
      fadeLight(stripeControl[i]);
    }
    if(stripeControl[i].status.FLASH == 1) {
      fadeFlashLight(stripeControl[i]);
    }
  }
  FastLED.show();
}

if (Serial.available()) { // Only do something if there's new data

    byte buf[2]; // Array for storing incoming data
    int index = 0; // How many bytes have been received
    while (index < 2 ){ // Wait until 2 bytes have been received
      if (Serial.available()) { 
        buf[index] = Serial.read();
        index++; 
      }
    }

    BS_LightEvent event = bslts.ParseMessage(buf);
    
    //setup 
    //toggle chroma with eventtype 255
    if(event.type == 255 && event.value == 1) {
      IsChroma = true;
      Serial.println("chroma enabled");
      return;
    }
    if(event.type == CHROMAEVENT && event.value == 0) {
      IsChroma = false;
      Serial.println("chroma disabled");
      return;
    }
    
    if(event.type == LEFTCOLOR) {
      leftColor = CRGB(event.color.r, event.color.g, event.color.b);
      return;
    }
    if(event.type == RIGHTCOLOR) {
      rightColor = CRGB(event.color.r, event.color.g, event.color.b);
      return;
    }

    // stripe is split into 7 different sections. Change sections depending on eventtype
    switch (event.type)
      {
      case BACKTOPLASER : 
        controlLight(stripeControl[3], event);
        FastLED.show();
        break;
      case RINGLASER :
        controlLight(stripeControl[0], event);
        leds(stripeControl[6].MIN,stripeControl[6].MAX-1).operator=(leds[stripeControl[0].MIN]);
        stripeControl[6].color = stripeControl[0].color;
        stripeControl[6].status = stripeControl[0].status;
        //controlLight(stripeControl[6], event);
        FastLED.show();
        break;
      case LEFTLASER : controlLight(stripeControl[1], event);
        FastLED.show();
        break;
      case RIGHTLASER : controlLight(stripeControl[5], event);
        FastLED.show();
        break;
      case CENTERLIGHT : 
        controlLight(stripeControl[2], event);
        leds(stripeControl[4].MIN,stripeControl[4].MAX-1).operator=(leds[stripeControl[2].MIN]);
        stripeControl[4].color = stripeControl[2].color;
        stripeControl[4].status = stripeControl[2].status;
        FastLED.show();
        break;
      case LEFTLASERSPEED : leftLaser.laserSpeed = event.value;
        break;
      case RIGHTLASERSPEED : rightLaser.laserSpeed = event.value;
        break;
      default: ;
        break;
      }
      
  }
  //laser walkanimation
  //ledwalkleft(&leftLaser, &stripeControl[leftLaser.index].MIN, &stripeControl[leftLaser.index].MAX);
  //ledwalkright(&rightLaser, &stripeControl[rightLaser.index].MIN, &stripeControl[rightLaser.index].MAX);
  
}

//read Serial jdxsu

//light events control
void controlLight(struct Lights& l, BS_LightEvent event) {
  CRGB colorWithoutDivider;
  colorWithoutDivider.setRGB(event.color.r, event.color.g, event.color.b);
  //handle chroma
  if(IsChroma) {
    if(colorWithoutDivider.getAverageLight() != 0) {
      l.color = colorWithoutDivider;
      l.color = CRGB(l.color.r * BRIGHTNESSDEVIDER, l.color.g * BRIGHTNESSDEVIDER, l.color.b * BRIGHTNESSDEVIDER);
    }
  //default colors
  } else if(event.value > 0 && event.value < 4) {
    l.color = CRGB(rightColor);
    l.color = CRGB(l.color.r * BRIGHTNESSDEVIDER, l.color.g * BRIGHTNESSDEVIDER, l.color.b * BRIGHTNESSDEVIDER);
    colorWithoutDivider = CRGB(rightColor);
  } else if(event.value > 4 && event.value < 8) {
    l.color = CRGB(leftColor);
    l.color = CRGB(l.color.r * BRIGHTNESSDEVIDER, l.color.g * BRIGHTNESSDEVIDER, l.color.b * BRIGHTNESSDEVIDER);
    colorWithoutDivider = CRGB(leftColor);
  }
  switch (event.value)
  {
  case LIGHTOFF:
    l.status.ON = 0;
    l.status.FLASH = 0;
    l.status.FADE = 0;
    break;
  case LEFTCOLORON:
      l.status.ON = 1;
      l.status.FLASH = 0;
      l.status.FADE = 0;
    break;
  case LEFTCOLORFADE:
    l.status.ON = 1;
    l.status.FADE = 1;
    l.status.FLASH = 0;
    break;
  case LEFTCOLORFLASH:
    l.status.ON = 1;
    l.status.FLASH = 1;
    l.status.FADE = 0;
    break;
  case RIGHTCOLORON:
    l.status.ON = 1;
    l.status.FLASH = 0;
    l.status.FADE = 0;
    break;
  case RIGHTCOLORFADE:
    l.status.ON = 1;
    l.status.FADE = 1;
    l.status.FLASH = 0;
    break;
  case RIGHTCOLORFLASH:
    l.status.ON = 1;
    l.status.FLASH = 1;
    l.status.FADE = 0;
    break;
  default:
    break;
  }
  if(l.status.ON == 1) {
      if(l.status.FLASH == 0) {
        leds(l.MIN, l.MAX-1) = l.color;
        //leds[i] = l.color;

      } else {
        //take color without divider for flash
        leds(l.MIN, l.MAX-1) = colorWithoutDivider;
        //leds[i] = colorWithoutDivider;
      }
  } else {
      leds(l.MIN, l.MAX-1) = CRGB::Black;
  }
}

//fade function
void fadeLight(struct Lights& l) {
  
    leds(l.MIN, l.MAX-1).fadeToBlackBy(1); 
    if(leds[l.MIN].getAverageLight() <= 0) {
     // Serial.println(l.MIN);
     // Serial.println(l.MAX);
      leds(l.MIN, l.MAX-1) = CRGB::Black;
      l.status.FADE = 0;
      l.status.ON = 0;
    }
}
// fade until rgb value of  is reached
void fadeFlashLight(struct Lights& l) {
    leds(l.MIN, l.MAX-1).subFromRGB(1);
    if(leds[l.MIN] <= l.color) {
      l.status.FLASH = 0;
      leds(l.MIN, l.MAX-1) = l.color;
  }
}
void ledwalkleft(struct Laser *laser, int *min, int *max) {
  if(*min + laser->laserIndex == *max) {
    laser->laserIndex = 0;
    laser->toggle = !laser->toggle;
    }
  EVERY_N_MILLISECONDS_I(thistimer, 200 / laser->laserSpeed) {
  thistimer.setPeriod(200 - (laser->laserSpeed * 10));
    if(stripeControl[laser->index].status.ON == 1) {
      if(laser->toggle) {
      leds[*min + laser->laserIndex] = CRGB::Black;
      } else
      {
       leds[*min + laser->laserIndex] = stripeControl[laser->index].color;
      }
    } else {
      BS_LightEvent bl;
      bl.color.r = 0; bl.color.g = 0; bl.color.b = 0;
      bl.type = laser->index;
      bl.value = LIGHTOFF;
      controlLight(stripeControl[laser->index], bl);
    }
    
    laser->laserIndex++;
    FastLED.show();
    } 
  }
void ledwalkright(struct Laser *laser, int *min, int *max) {
  if(*max - laser->laserIndex == *min) {
    laser->laserIndex = 0;
    laser->toggle = !laser->toggle;
    }
  EVERY_N_MILLISECONDS_I(thistimer, 200 / laser->laserSpeed) {
    thistimer.setPeriod(200 - (laser->laserSpeed * 10));
    if(stripeControl[laser->index].status.ON == 1) {
      if(laser->toggle) {
        leds[*max - laser->laserIndex] = CRGB::Black;
      } else
      {
        leds[*max - laser->laserIndex] = stripeControl[laser->index].color;
      }
    } else {
      BS_LightEvent bl;
      bl.color.r = 0; bl.color.g = 0; bl.color.b = 0;
      bl.type = laser->index;
      bl.value = LIGHTOFF;
      controlLight(stripeControl[laser->index], bl);
    }
      
    laser->laserIndex++;
    FastLED.show();
    } 
  }


