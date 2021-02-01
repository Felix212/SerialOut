#include "main.hpp"

// global vars
t_cached_colors cached_colors;

bool IsChroma = false;

CRGB leds[TOTAL_LEDS];
CRGBArray<TOTAL_LEDS> support_array;

t_light_controller stripeControl[7];

t_laser leftLaser;
t_laser rightLaser;

uint32_t current_mills_cached;

uint32_t frame_time_start_millis;
uint32_t fade_time_start;

uint32_t laser_left_timer_start;
uint32_t laser_right_timer_start;
uint32_t laser_left_time_update;
uint32_t laser_right_time_update;

//light events control
CRGB chromaColor; // defined outside to not declare every event
LightEvents current_LightEvent;
t_light_controller *current_handler;

byte event_buffer[2];
byte ev_0;
byte ev_1;
byte event_type;
byte event_value;
int handler_index;

void setup()
{
  cacheLeftColor(defaultColorLEFT.r, defaultColorLEFT.g, defaultColorLEFT.b);
  cacheRightColor(defaultColorRIGHT.r, defaultColorRIGHT.g, defaultColorRIGHT.b);

  init_controllers();

  init_lasers();

  // FastLED.addLeds<LEDTYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.addLeds<LEDTYPE, PIN_LED_STRIP_1>(leds, 0, TOTAL_LEDS_STRIP_1);
  FastLED.addLeds<LEDTYPE, PIN_LED_STRIP_2>(leds, TOTAL_LEDS_STRIP_1, TOTAL_LEDS_STRIP_2);

  if (BRIGHTNESS != 255)
  {
    FastLED.setBrightness(BRIGHTNESS);
  }

  if (POWERLIMIT != -1)
  {
    FastLED.setMaxPowerInMilliWatts(POWERLIMIT);
  }

  frame_time_start_millis = 0;
  fade_time_start = 0;

  Serial.begin(BAUD_RATE);

  checkLeds(leds, 0, TOTAL_LEDS);
}

void reset_controllers()
{
  for (int i = 0; i < 7; i++)
  {
    stripeControl[i].status.ON = 0;
    stripeControl[i].status.FADE = 0;
    stripeControl[i].status.FLASH = 0;
    stripeControl[i].color = CRGB::Black;
    stripeControl[i].color_flash = CRGB::Black;
    stripeControl[i].actual_color = CRGB::Black;
  }
}

void init_controllers()
{
  int valuesMinMax[] = {LEDSTART, LEDSPLIT1, LEDSPLIT2, LEDSPLIT3, LEDSPLIT4, LEDSPLIT5, LEDSPLIT6, LEDEND};
  for (int i = 0; i < 7; i++)
  {
    stripeControl[i].from = valuesMinMax[i];
    stripeControl[i].to = valuesMinMax[i + 1];
  }
  reset_controllers();
}

void init_lasers()
{
  leftLaser.strip_part_index = 1;
  rightLaser.strip_part_index = 5;
  laser_left_timer_start = 0;
  laser_right_timer_start = 0;
  laser_left_time_update = 151 / leftLaser.laserSpeed;
  laser_right_time_update = 151 / rightLaser.laserSpeed;
}

void cacheLeftColor(uint8_t r, uint8_t g, uint8_t b)
{
  cached_colors.color_left_flash = CRGB(r, g, b);
  cached_colors.color_left = CRGB((uint8_t)(r * BRIGHTNESSDIVIDER),
                                  (uint8_t)(g * BRIGHTNESSDIVIDER),
                                  (uint8_t)(b * BRIGHTNESSDIVIDER));
}

void cacheRightColor(uint8_t r, uint8_t g, uint8_t b)
{
  cached_colors.color_right_flash = CRGB(r, g, b);
  cached_colors.color_right = CRGB((uint8_t)(r * BRIGHTNESSDIVIDER),
                                   (uint8_t)(g * BRIGHTNESSDIVIDER),
                                   (uint8_t)(b * BRIGHTNESSDIVIDER));
}

void show_frame()
{
  if (current_mills_cached - frame_time_start_millis > TIME_BETWEEN_UPDATES)
  {
    // update leds with current settings
    int i, k = 0;
    for (i = TOTAL_LEDS_STRIP_1 - 1; i >= 0; --i)
    {
      leds[i] = support_array[k];
      k = k + 1;
    }
    for (i = TOTAL_LEDS_STRIP_1; i < TOTAL_LEDS; ++i)
    {
      leds[i] = support_array[k];
      k = k + 1;
    }

    // Show updates
    FastLED.show();
    frame_time_start_millis = current_mills_cached;
  }
}

void fade_leds()
{
  if (current_mills_cached - fade_time_start > FADE_TIME_MILLIS)
  {
    for (int i = 0; i < 7; ++i)
    {
      if (stripeControl[i].status.FLASH == 1)
      {
        handler_index = i;
        fadeFlashLight();
      }
      else if (stripeControl[i].status.FADE == 1)
      {
        handler_index = i;
        fadeLight();
      }
    }
    fade_time_start = current_mills_cached;
  }
}

void loop()
{
  current_mills_cached = millis();

  // Only do something if there's new data
  while (Serial.available())
  {
    Serial.readBytes(event_buffer, 2);
    ev_0 = event_buffer[0];
    ev_1 = event_buffer[1];

    if (ev_0 > 240)
    {
      switch (ev_0)
      {
      case TURNOFFLIGHTS:
        support_array(0, LEDEND - 1) = CRGB::Black;
        reset_controllers();
        break;
      case LEFTCOLOR:
        cacheLeftColor((ev_1 >> 5) * 32,        // red
                       ((ev_1 & 28) >> 2) * 32, // green
                       (ev_1 & 3) * 64);        // blue
        break;
      case RIGHTCOLOR:
        cacheRightColor((ev_1 >> 5) * 32,        // red
                        ((ev_1 & 28) >> 2) * 32, // green
                        (ev_1 & 3) * 64);        // blue
        break;
      case CHROMAEVENT:
        if (ev_1 == 1)
        {
          IsChroma = true;
        }
        else if (ev_1 == 0)
        {
          IsChroma = false;
        }
        break;
      default:
        break;
      }
    }
    else
    {
      event_value = (ev_0 & 15);
      if (IsChroma)
      {
        chromaColor = CRGB((ev_1 >> 5) * 32,        // red
                           ((ev_1 & 28) >> 2) * 32, // green
                           (ev_1 & 3) * 64);        // blue
      }

      switch ((ev_0 >> 4))
      {
      case BACKTOPLASER:
        handler_index = 3;
        controlLight();
        break;
      case RINGLASER:
        handler_index = 0;
        controlLight();
        handler_index = 6;
        controlLight();
        break;
      case LEFTLASER:
        handler_index = 1;
        controlLight();
        break;
      case RIGHTLASER:
        handler_index = 5;
        controlLight();
        break;
      case CENTERLIGHT:
        handler_index = 2;
        controlLight();
        handler_index = 4;
        controlLight();
        break;
      // Laser configuration Events
      case LEFTLASERSPEED:
        leftLaser.laserSpeed = (ev_0 & 15);
        laser_left_time_update = 151 / leftLaser.laserSpeed;
        break;
      case RIGHTLASERSPEED:
        rightLaser.laserSpeed = (ev_0 & 15);
        laser_right_time_update = 151 / rightLaser.laserSpeed;
        break;
      default:
        break;
      }
    }
  }

  fade_leds();

  //laser walkanimation
  ledwalkleft(&leftLaser);
  ledwalkright(&rightLaser);

  // coloring actual leds
  show_frame();
}

void controlLight()
{
  current_handler = &stripeControl[handler_index];
  current_LightEvent = (LightEvents)event_value;

  //handle chroma
  if (IsChroma)
  {
    if (chromaColor.getAverageLight() != 0)
    {
      current_handler->color_flash.setRGB(chromaColor.r,
                                          chromaColor.g,
                                          chromaColor.b);
      current_handler->color.setRGB((uint8_t)((float)chromaColor.r * BRIGHTNESSDIVIDER),
                                    (uint8_t)((float)chromaColor.g * BRIGHTNESSDIVIDER),
                                    (uint8_t)((float)chromaColor.b * BRIGHTNESSDIVIDER));
    }
  }
  //default colors
  else
  {
    if (event_value > 0 && event_value < 4)
    {
      current_handler->color_flash = cached_colors.color_right_flash;
      current_handler->color = cached_colors.color_right;
    }

    if (event_value > 4 && event_value < 8)
    {
      current_handler->color_flash = cached_colors.color_left_flash;
      current_handler->color = cached_colors.color_left;
    }
  }

  switch (current_LightEvent)
  {
  case LightEvents::Light_Off:
    current_handler->status.ON = 0;
    current_handler->status.FLASH = 0;
    current_handler->status.FADE = 0;
    break;
  case LightEvents::Right_Color_On:
  case LightEvents::Left_Color_On:
    current_handler->status.ON = 1;
    current_handler->status.FADE = 0;
    current_handler->status.FLASH = 0;
    break;
  case LightEvents::Right_Color_Flash:
  case LightEvents::Left_Color_Flash:
    current_handler->status.ON = 1;
    current_handler->status.FADE = 0;
    current_handler->status.FLASH = 1;
    break;
  case LightEvents::Right_Color_Fade:
  case LightEvents::Left_Color_Fade:
    current_handler->status.ON = 1;
    current_handler->status.FADE = 1;
    current_handler->status.FLASH = 0;
    break;
  default:
    break;
  }

  /* Updates LEDS color */
  if (current_handler->status.ON == 1)
  {
    if (current_handler->status.FLASH == 1)
    {
      //take color without divider for flash
      if (handler_index != 1 && handler_index != 5) // if not laser
      {
        support_array(current_handler->from, current_handler->to - 1) = current_handler->color_flash;
      }
      current_handler->actual_color = current_handler->color_flash;
    }
    else
    {
      if (handler_index != 1 && handler_index != 5) // if not laser
      {
        support_array(current_handler->from, current_handler->to - 1) = current_handler->color;
      }
      current_handler->actual_color = current_handler->color;
    }
  }
  else
  {
    support_array(current_handler->from, current_handler->to - 1) = CRGB::Black;
    current_handler->actual_color = CRGB::Black;
  }
}

//fade function
void fadeLight()
{
  current_handler = &stripeControl[handler_index];
  current_handler->actual_color = current_handler->actual_color.fadeToBlackBy(1);

  if (current_handler->actual_color.getAverageLight() > 16)
  {
    if (handler_index != 1 && handler_index != 5) // if not laser
    {
      support_array(current_handler->from, current_handler->to - 1) = current_handler->actual_color;
    }
  }
  else
  {
    support_array(current_handler->from, current_handler->to - 1) = CRGB::Black;
    current_handler->actual_color = CRGB::Black;
    current_handler->status.FADE = 0;
    current_handler->status.ON = 0;
  }
}

// fade until rgb value of  is reached
void fadeFlashLight()
{
  current_handler = &stripeControl[handler_index];
  current_handler->actual_color = current_handler->actual_color.fadeToBlackBy(1);

  if (current_handler->actual_color.getLuma() > current_handler->color.getLuma())
  {
    if (handler_index != 1 && handler_index != 5) // if not laser
    {
      support_array(current_handler->from, current_handler->to - 1) = current_handler->actual_color;
    }
  }
  else
  {
    if (handler_index != 1 && handler_index != 5) // if not laser
    {
      support_array(current_handler->from, current_handler->to - 1) = current_handler->color;
    }
    current_handler->actual_color = current_handler->color;
    current_handler->status.FLASH = 0;
  }
}

void ledwalkleft(struct Laser *laser)
{
  current_handler = &stripeControl[laser->strip_part_index];

  if (current_handler->status.ON == 0)
  {
    return;
  }

  if (current_handler->from + laser->laserIndex == current_handler->to)
  {
    laser->laserIndex = 0;
    laser->toggle = !laser->toggle;
  }

  if (current_mills_cached - laser_left_timer_start > laser_left_time_update)
  {
    if (laser->toggle)
    {
      support_array[current_handler->from + laser->laserIndex] = CRGB::Black;
    }
    else
    {
      support_array[current_handler->from + laser->laserIndex] = stripeControl[laser->strip_part_index].actual_color;
    }

    laser->laserIndex++;
    laser_left_timer_start = current_mills_cached;
  }
}

void ledwalkright(struct Laser *laser)
{
  current_handler = &stripeControl[laser->strip_part_index];

  if (current_handler->status.ON == 0)
  {
    return;
  }

  if (current_handler->to - laser->laserIndex - 1 < current_handler->from)
  {
    laser->laserIndex = 0;
    laser->toggle = !laser->toggle;
  }

  if (current_mills_cached - laser_right_timer_start > laser_right_time_update)
  {
    if (laser->toggle)
    {
      support_array[current_handler->to - laser->laserIndex - 1] = CRGB::Black;
    }
    else
    {
      support_array[current_handler->to - laser->laserIndex - 1] = stripeControl[laser->strip_part_index].actual_color;
    }

    laser->laserIndex++;
    laser_right_timer_start = current_mills_cached;
  }
}
