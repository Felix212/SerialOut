#include <Arduino.h>
#include <FastLED.h>
#include <BS_LightToSerial.h>
#include "main.hpp"

// global vars
CRGB rightColor;
CRGB leftColor;
bool IsChroma = false;
CRGB leds[NUM_LEDS];
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
  int valuesMinMax[] = {L1_MIN, L2_MIN, L3_MIN, L4_MIN, L5_MIN, L6_MIN, L7_MIN, L7_MAX};
  for (size_t i = 0; i < 7; i++)
  {
    stripeControl[i].MIN = valuesMinMax[i];
    stripeControl[i].MAX = valuesMinMax[i+1];
  }
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
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
for (int i = 0; i < 7; i++)
{
  if(stripeControl[i].FADE == 1) {
    fadeLight(stripeControl[i], i);
  }
  if(stripeControl[i].FLASH == 1) {
    fadeFlashLight(stripeControl[i], i);
  }
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
    
    //setup chroma
    //toggle chroma with eventtype 255
    if(event.type == 255 && event.value == 1) {
      IsChroma = true;
      Serial.println("chroma enabled");
      return;
    }
    if(event.type == 255 && event.value == 0) {
      IsChroma = false;
      Serial.println("chroma disabled");

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
        controlLight(stripeControl[6], event);
        FastLED.show();
        break;
      case LEFTLASER : controlLight(stripeControl[1], event);
        break;
      case RIGHTLASER : controlLight(stripeControl[5], event);
        break;
      case CENTERLIGHT : 
        controlLight(stripeControl[2], event);
        controlLight(stripeControl[4], event);
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
  ledwalkleft(&leftLaser, &stripeControl[leftLaser.index].MIN, &stripeControl[leftLaser.index].MAX);
  ledwalkright(&rightLaser, &stripeControl[rightLaser.index].MIN, &stripeControl[rightLaser.index].MAX);
  
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
    l.ON = 0;
    break;
  case LEFTCOLORON:
      l.ON = 1;
    break;
  case LEFTCOLORFADE:
    l.ON = 1;
    l.FADE = 1;
    break;
  case LEFTCOLORFLASH:
    l.ON = 1;
    l.FLASH = 1;
    break;
  case RIGHTCOLORON:
    l.ON = 1;
    break;
  case RIGHTCOLORFADE:
    l.ON = 1;
    l.FADE = 1;
    break;
  case RIGHTCOLORFLASH:
    l.ON = 1;
    l.FLASH = 1;
    break;
  default:
    break;
  }
  if(l.ON == 1) {
    for (size_t i = l.MIN; i < l.MAX; i++){
      if(l.FLASH == 0) {
        leds[i] = l.color;
      } else {
        //take color without divider for flash
        leds[i] = colorWithoutDivider;
      }
    }
  } else {
    for(size_t i = l.MIN; i < l.MAX; i++) {
      leds[i] = CRGB::Black;
    }
  }
}

//fade function
void fadeLight(struct Lights l, int id) {
  
   EVERY_N_MILLISECONDS(2) {
    for (size_t i = l.MIN; i < l.MAX; i++) {
      leds[i].fadeToBlackBy(1);
    }   
    FastLED.show();
    if(leds[l.MIN].getAverageLight() == 0) {
      stripeControl[id].FADE = 0;
      for (size_t i = l.MIN; i < l.MAX; i++)
      {
        leds[i] = CRGB::Black;
      }
      stripeControl[id].ON = 0;
      FastLED.show();
    }
    }
    
}
// fade until rgb value of  is reached
void fadeFlashLight(struct Lights l, int id) {
  
  EVERY_N_MILLISECONDS(6) {

    for (size_t i = l.MIN; i < l.MAX; i++) {
      leds[i].subtractFromRGB(1);
    }   
    FastLED.show();
    if(leds[l.MIN] <= l.color) {
      stripeControl[id].FLASH = 0;
      for (size_t i = l.MIN; i < l.MAX; i++)
      {
        leds[i] = l.color;
      }
      FastLED.show();
    }
    }
    
}
void ledwalkleft(struct Laser *laser, int *min, int *max) {
  if(*min + laser->laserIndex == *max) {
    laser->laserIndex = 0;
    laser->toggle = !laser->toggle;
    }
  EVERY_N_MILLISECONDS_I(thistimer, 200 / laser->laserSpeed) {
  thistimer.setPeriod(200 - (laser->laserSpeed * 10));
    if(stripeControl[laser->index].ON == 1) {
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
    if(stripeControl[laser->index].ON == 1) {
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


