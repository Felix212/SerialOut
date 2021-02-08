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
bool handling_laser;
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
  for (int i = 0; i < 7; ++i)
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
  int valuesMinMax[] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (CUSTOM_SPLITS)
  {
    valuesMinMax[0] = LEDSTART;
    valuesMinMax[1] = LEDSPLIT1;
    valuesMinMax[2] = LEDSPLIT2;
    valuesMinMax[3] = LEDSPLIT3;
    valuesMinMax[4] = LEDSPLIT4;
    valuesMinMax[5] = LEDSPLIT5;
    valuesMinMax[6] = LEDSPLIT6;
    valuesMinMax[7] = LEDEND;
  }
  else
  {
    int increment = TOTAL_LEDS / 7;
    int missing_leds = TOTAL_LEDS - increment * 7;

    for (size_t i = 1; i < 8; ++i)
    {
      valuesMinMax[i] = valuesMinMax[i - 1] + increment;
    }

    switch (missing_leds)
    {
      // BackTopLaser
    case 1:
      valuesMinMax[1] += 0;
      valuesMinMax[2] += 0;
      valuesMinMax[3] += 0;
      valuesMinMax[4] += 1;
      valuesMinMax[5] += 1;
      valuesMinMax[6] += 1;
      valuesMinMax[7] += 1;
      break;
      // Both Lasers
    case 2:
      valuesMinMax[1] += 0;
      valuesMinMax[2] += 1;
      valuesMinMax[3] += 1;
      valuesMinMax[4] += 1;
      valuesMinMax[5] += 1;
      valuesMinMax[6] += 2;
      valuesMinMax[7] += 2;
      break;
      // Both Lasers - BackTopLaser
    case 3:
      valuesMinMax[1] += 0;
      valuesMinMax[2] += 1;
      valuesMinMax[3] += 1;
      valuesMinMax[4] += 2;
      valuesMinMax[5] += 2;
      valuesMinMax[6] += 3;
      valuesMinMax[7] += 3;
      break;
    case 4:
      // Both Lasers - Both Center Lights
      valuesMinMax[1] += 0;
      valuesMinMax[2] += 1;
      valuesMinMax[3] += 2;
      valuesMinMax[4] += 2;
      valuesMinMax[5] += 3;
      valuesMinMax[6] += 4;
      valuesMinMax[7] += 4;
      break;
      // Both Lasers - Both Center Lights - BackTopLaser
    case 5:
      valuesMinMax[1] += 0;
      valuesMinMax[2] += 1;
      valuesMinMax[3] += 2;
      valuesMinMax[4] += 3;
      valuesMinMax[5] += 4;
      valuesMinMax[6] += 5;
      valuesMinMax[7] += 5;
      break;
      // Both Lasers - Both Center Lights - Both RingLights
    case 6:
      valuesMinMax[1] += 1;
      valuesMinMax[2] += 2;
      valuesMinMax[3] += 3;
      valuesMinMax[4] += 3;
      valuesMinMax[5] += 4;
      valuesMinMax[6] += 5;
      valuesMinMax[7] += 6;
      break;

    default:
      break;
    }
  }

  for (int i = 0; i < 7; ++i)
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
    update_leds(leds, support_array);

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
      case SETUPEVENTS:
        break;
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
      event_type = ev_0 >> 4;
      event_value = ev_0 & 15;
      if (IsChroma)
      {
        chromaColor = CRGB((ev_1 >> 5) * 32,        // red
                           ((ev_1 & 28) >> 2) * 32, // green
                           (ev_1 & 3) * 64);        // blue
      }

      switch (event_type)
      {
      case BACKTOPLASER:
        handler_index = INDEX_BACK_TOP_LASER;
        controlLight();
        break;
      case RINGLASER:
        handler_index = INDEX_RING_LASER_LEFT;
        controlLight();
        handler_index = INDEX_RING_LASER_RIGHT;
        controlLight();
        break;
      case LEFTLASER:
        handler_index = INDEX_LEFT_LASER;
        handling_laser = true;
        controlLight();
        handling_laser = false;
        break;
      case RIGHTLASER:
        handler_index = INDEX_RIGHT_LASER;
        handling_laser = true;
        controlLight();
        handling_laser = false;
        break;
      case CENTERLIGHT:
        handler_index = INDEX_CENTER_LIGHT_LEFT;
        controlLight();
        handler_index = INDEX_CENTER_LIGHT_RIGHT;
        controlLight();
        break;
      // Laser configuration Events
      case LEFTLASERSPEED:
        leftLaser.laserSpeed = event_value;
        laser_left_time_update = 151 / leftLaser.laserSpeed;
        break;
      case RIGHTLASERSPEED:
        rightLaser.laserSpeed = event_value;
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

  // if minimum time passed of led on
  for (int i = 0; i < 7; ++i)
  {
    if (stripeControl[i].have_to_turn_off)
    {
      if (current_mills_cached - stripeControl[i].turn_up_millis > MINIUM_TURN_ON_LIGHT_TIME)
      {
        stripeControl[i].have_to_turn_off = false;
        support_array(stripeControl[i].from, stripeControl[i].to - 1) = CRGB::Black;
        stripeControl[i].actual_color = CRGB::Black;
        if (i == INDEX_LEFT_LASER) // left laser
        {
          leftLaser.laserIndex = 0;
        }
        else if (i == INDEX_RIGHT_LASER) // right laser
        {
          rightLaser.laserIndex = 0;
        }
      }
    }
  }

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
    current_handler->turn_up_millis = current_mills_cached;
    if (current_handler->status.FLASH == 1)
    {
      current_handler->actual_color = current_handler->color_flash;
    }
    else
    {
      current_handler->actual_color = current_handler->color;
    }

    // Update color of turned on leds
    if (handling_laser)
    {
      for (int i = current_handler->from; i < current_handler->to; ++i)
      {
        if (support_array[i].getAverageLight() > 16) // not black or dark
        {
          support_array[i] = current_handler->actual_color;
        }
      }
    }
    else
    {
      support_array(current_handler->from, current_handler->to - 1) = current_handler->actual_color;
    }
  }
  else
  {
    current_handler->have_to_turn_off = true;
  }
}

//fade function
void fadeLight()
{
  current_handler = &stripeControl[handler_index];
  current_handler->actual_color = current_handler->actual_color.fadeToBlackBy(1);

  if (current_handler->actual_color.getAverageLight() > 16)
  {
    if (handler_index != INDEX_LEFT_LASER && handler_index != INDEX_RIGHT_LASER) // if not laser
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
    if (handler_index != INDEX_LEFT_LASER && handler_index != INDEX_RIGHT_LASER) // if not laser
    {
      support_array(current_handler->from, current_handler->to - 1) = current_handler->actual_color;
    }
  }
  else
  {
    if (handler_index != INDEX_LEFT_LASER && handler_index != INDEX_RIGHT_LASER) // if not laser
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
