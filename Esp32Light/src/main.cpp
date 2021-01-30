#include "main.hpp"

// global vars
t_colors cached_colors;

bool IsChroma = false;

CRGBArray<NUM_LEDS> leds;
struct Lights stripeControl[7];
Laser leftLaser;
Laser rightLaser;
BS_LightToSerial bslts;

void setup()
{
  cacheColors(bslts.left, bslts.right);

  //setup serial
  Serial.begin(BAUD_RATE);

  int valuesMinMax[] = {LEDSTART, LEDSPLIT1, LEDSPLIT2, LEDSPLIT3, LEDSPLIT4, LEDSPLIT5, LEDSPLIT6, LEDEND};
  for (int i = 0; i < 7; i++)
  {
    stripeControl[i].MIN = valuesMinMax[i];
    stripeControl[i].MAX = valuesMinMax[i + 1];
  }

  // FastLED.addLeds<LEDTYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.addLeds<LEDTYPE, DATA_PIN>(leds, NUM_LEDS);

  FastLED.setBrightness(BRIGHTNESS);
  if (POWERLIMIT != -1)
  {
    FastLED.setMaxPowerInMilliWatts(POWERLIMIT);
  }

  checkLeds();

  leftLaser.index = 1;
  rightLaser.index = 5;
}

void cacheColors(color left, color right)
{
  CRGB left_normal_color = CRGB((uint8_t)((float)left.r * BRIGHTNESSDIVIDER),
                                (uint8_t)((float)left.g * BRIGHTNESSDIVIDER),
                                (uint8_t)((float)left.b * BRIGHTNESSDIVIDER));
  CRGB right_normal_color = CRGB((uint8_t)((float)right.r * BRIGHTNESSDIVIDER),
                                 (uint8_t)((float)right.g * BRIGHTNESSDIVIDER),
                                 (uint8_t)((float)right.b * BRIGHTNESSDIVIDER));

  cached_colors.left = left;

  cached_colors.right = right;

  cached_colors.color_left_flash.setRGB(left.r,
                                        left.g,
                                        left.b);

  cached_colors.color_right_flash.setRGB(right.r,
                                         right.g,
                                         right.b);

  cached_colors.color_left.setRGB(left_normal_color.r,
                                  left_normal_color.g,
                                  left_normal_color.b);

  cached_colors.color_right.setRGB(right_normal_color.r,
                                   right_normal_color.g,
                                   right_normal_color.b);
}

byte received_bytes_counter;
// Array for storing incoming data
byte buffer_data_input[2];

void loop()
{
  /* */
  //Fade in progress?
  EVERY_N_MILLISECONDS(TIME_BETWEEN_UPDATES)
  {

    for (int i = 0; i < 7; ++i)
    {
      if (stripeControl[i].status.FADE == 1)
      {
        fadeLight(stripeControl[i]);
      }
      if (stripeControl[i].status.FLASH == 1)
      {
        fadeFlashLight(stripeControl[i]);
      }
    }

    //update all leds
    FastLED.show();
  }

  if (Serial.available()) // Only do something if there's new data
  {
    received_bytes_counter = 0; // How many bytes have been received
    while (received_bytes_counter < 2)
    { // Wait until 2 bytes have been received
      if (Serial.available())
      {
        buffer_data_input[received_bytes_counter] = Serial.read();
        received_bytes_counter++;
      }
    }

    BS_LightEvent event = bslts.ParseMessage(buffer_data_input);

    if (event.type == CHROMAEVENT)
    {
      if (event.value == 1)
      {
        IsChroma = true;
        if (DEBUG_MESSAGES)
        {
          Serial.println("chroma enabled");
        }
      }
      else if (event.value == 0)
      {
        IsChroma = false;
        if (DEBUG_MESSAGES)
        {
          Serial.println("chroma disabled");
        }
      }
      return;
    }

    //get note colors
    if (event.type == LEFTCOLOR)
    {
      cacheColors(event.color, cached_colors.right);
      return;
    }
    if (event.type == RIGHTCOLOR)
    {
      cacheColors(cached_colors.left, event.color);
      return;
    }
    //reset lights
    if (event.type == TURNOFFLIGHTS)
    {
      leds(0, LEDEND - 1) = CRGB::Black;
    }

    // stripe is split into 7 different sections. Change sections depending on eventtype
    switch (event.type)
    {
    case BACKTOPLASER:
      controlLight(stripeControl[3], event);
      break;
    case RINGLASER:
      controlLight(stripeControl[0], event);
      controlLight(stripeControl[6], event);
      break;
    case LEFTLASER:
      controlLight(stripeControl[1], event);
      break;
    case RIGHTLASER:
      controlLight(stripeControl[5], event);
      break;
    case CENTERLIGHT:
      controlLight(stripeControl[2], event);
      controlLight(stripeControl[4], event);
      break;
    case LEFTLASERSPEED:
      leftLaser.laserSpeed = event.value;
      break;
    case RIGHTLASERSPEED:
      rightLaser.laserSpeed = event.value;
      break;
    default:
      break;
    }
  }

  //laser walkanimation
  ledwalkleft(&leftLaser, &stripeControl[leftLaser.index].MIN, &stripeControl[leftLaser.index].MAX);
  ledwalkright(&rightLaser, &stripeControl[rightLaser.index].MIN, &stripeControl[rightLaser.index].MAX);
}

// Checks leds by turning all them on with colors RED -> GREEN -> BLUE
void checkLeds()
{
  //Led check
  leds(0, LEDEND - 1) = CRGB::Black;
  FastLED.show();
  leds(0, LEDEND - 1) = CRGB::Red;
  FastLED.show();
  delay(500);
  leds(0, LEDEND - 1) = CRGB::Green;
  FastLED.show();
  delay(500);
  leds(0, LEDEND - 1) = CRGB::Blue;
  FastLED.show();
  delay(500);
  leds(0, LEDEND - 1) = CRGB::Black;
  FastLED.show();
}

//light events control
CRGB chromaColor; // defined outside to not declare every event

void controlLight(struct Lights &l, BS_LightEvent event)
{
  if (DEBUG_MESSAGES)
  {
    Serial.println("Handling light");
    Serial.println("Message Received:");
    Serial.print("Type: ");
    Serial.println(event.type);
    Serial.print("Value: ");
    Serial.println(event.value);
    Serial.print("Color: ");
    Serial.println("(" + String(event.color.r) + "," + String(event.color.g) + "," + String(event.color.b) + ")");
    Serial.print("Current light color: ");
    Serial.println("(" + String(l.color.r) + "," + String(l.color.g) + "," + String(l.color.b) + ")");
  }

  //handle chroma
  if (IsChroma)
  {
    chromaColor = CRGB(event.color.r,
                       event.color.g,
                       event.color.b);
    if (chromaColor.getAverageLight() != 0)
    {
      l.color_flash.setRGB(chromaColor.r,
                           chromaColor.g,
                           chromaColor.b);
      l.color.setRGB((uint8_t)((float)chromaColor.r * BRIGHTNESSDIVIDER),
                     (uint8_t)((float)chromaColor.g * BRIGHTNESSDIVIDER),
                     (uint8_t)((float)chromaColor.b * BRIGHTNESSDIVIDER));
    }
  }
  //default colors
  else
  {
    if (event.value > 0 && event.value < 4)
    {
      l.color_flash = cached_colors.color_right_flash;
      l.color = cached_colors.color_right;
    }

    if (event.value > 4 && event.value < 8)
    {
      l.color_flash = cached_colors.color_left_flash;
      l.color = cached_colors.color_left;
    }
  }

  switch (event.value)
  {
  case BS_LightToSerial::LightEvents::Light_Off:
    l.status.ON = 0;
    l.status.FLASH = 0;
    l.status.FADE = 0;
    break;
  case BS_LightToSerial::LightEvents::Right_Color_On:
  case BS_LightToSerial::LightEvents::Left_Color_On:
    l.status.ON = 1;
    l.status.FADE = 0;
    l.status.FLASH = 0;
    break;
  case BS_LightToSerial::LightEvents::Right_Color_Flash:
  case BS_LightToSerial::LightEvents::Left_Color_Flash:
    l.status.ON = 1;
    l.status.FADE = 0;
    l.status.FLASH = 1;
    break;
  case BS_LightToSerial::LightEvents::Right_Color_Fade:
  case BS_LightToSerial::LightEvents::Left_Color_Fade:
    l.status.ON = 1;
    l.status.FADE = 1;
    l.status.FLASH = 0;
    break;
  default:
    break;
  }

  /* Updates LEDS color */
  if (l.status.ON == 1)
  {
    if (l.status.FLASH == 1)
    {
      //take color without divider for flash
      leds(l.MIN, l.MAX - 1) = l.color_flash;
    }
    else
    {
      leds(l.MIN, l.MAX - 1) = l.color;
    }
  }
  else
  {
    leds(l.MIN, l.MAX - 1) = CRGB::Black;
  }
}

//fade function
void fadeLight(struct Lights &l)
{
  leds(l.MIN, l.MAX - 1).fadeToBlackBy(1);
  if (leds[l.MIN].getAverageLight() <= 0)
  {
    leds(l.MIN, l.MAX - 1) = CRGB::Black;
    l.status.FADE = 0;
    l.status.ON = 0;
  }
}

// fade until rgb value of  is reached
void fadeFlashLight(struct Lights &l)
{
  leds(l.MIN, l.MAX - 1)--;
  if (leds[l.MIN].getLuma() <= l.color.getLuma())
  {
    l.status.FLASH = 0;
    leds(l.MIN, l.MAX - 1) = l.color;
  }
}

//uhhhmmmm refactoring needed
void ledwalkleft(struct Laser *laser, int *min, int *max)
{
  if (*min + laser->laserIndex == *max)
  {
    laser->laserIndex = 0;
    laser->toggle = !laser->toggle;
  }
  EVERY_N_MILLISECONDS_I(thistimer, 151 / laser->laserSpeed)
  {
    thistimer.setPeriod(151 - (laser->laserSpeed * 10));
    if (stripeControl[laser->index].status.ON == 1)
    {
      if (laser->toggle)
      {
        leds[*min + laser->laserIndex] = CRGB::Black;
      }
      else
      {
        leds[*min + laser->laserIndex] = stripeControl[laser->index].color;
      }
    }
    else
    {
      BS_LightEvent bl;
      bl.color.r = 0;
      bl.color.g = 0;
      bl.color.b = 0;
      bl.type = laser->index;
      bl.value = BS_LightToSerial::LightEvents::Light_Off;
      controlLight(stripeControl[laser->index], bl);
    }
    laser->laserIndex++;
  }
}
void ledwalkright(struct Laser *laser, int *min, int *max)
{
  if (*max - laser->laserIndex == *min)
  {
    laser->laserIndex = 0;
    laser->toggle = !laser->toggle;
  }
  EVERY_N_MILLISECONDS_I(thistimer, 151 / laser->laserSpeed)
  {
    thistimer.setPeriod(151 - (laser->laserSpeed * 10));
    if (stripeControl[laser->index].status.ON == 1)
    {
      if (laser->toggle)
      {
        leds[*max - laser->laserIndex] = CRGB::Black;
      }
      else
      {
        leds[*max - laser->laserIndex] = stripeControl[laser->index].color;
      }
    }
    else
    {
      BS_LightEvent bl;
      bl.color.r = 0;
      bl.color.g = 0;
      bl.color.b = 0;
      bl.type = laser->index;
      bl.value = BS_LightToSerial::LightEvents::Light_Off;
      controlLight(stripeControl[laser->index], bl);
    }
    laser->laserIndex++;
  }
}
