#include "main.hpp"

// global vars
t_cached_colors cached_colors;

bool IsChroma = false;

CRGB leds[TOTAL_LEDS];
CRGBArray<TOTAL_LEDS> support_array;

size_t total_controllers = 7;
LightController *stripeControllers[7];

t_laser leftLaser;
t_laser rightLaser;

uint32_t laser_left_timer_start;
uint32_t laser_right_timer_start;
uint32_t laser_left_time_update;
uint32_t laser_right_time_update;

uint32_t current_mills_cached;
uint32_t frame_time_start_millis;

LightToSerialParser *parser;
t_lightEvent *current_event;
t_status event_status;

void setup()
{
  parser = new LightToSerialParser();

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

  Serial.begin(BAUD_RATE);

  checkLeds(leds, 0, TOTAL_LEDS);
}

void reset_controllers()
{
  for (int i = 0; i < 7; ++i)
  {
    stripeControllers[i]->reset_status();
    stripeControllers[i]->reset_colors();
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
    stripeControllers[i] = new LightController(&support_array, valuesMinMax[i], valuesMinMax[i+1]);
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

void loop()
{
  current_mills_cached = millis();

  // Handle events only if there is new data
  while (parser->moreEvents())
  {
    parser->readData();
    current_event = parser->parseMessage();
    handleEvent();
    free(current_event);
  }

  for (size_t i = 0; i < total_controllers; ++i)
  {
    stripeControllers[i]->update(current_mills_cached);
  }

  // coloring actual leds
  if (current_mills_cached - frame_time_start_millis > TIME_BETWEEN_UPDATES)
  {
    // update leds with current settings
    update_leds(leds, support_array);

    // Show updates
    FastLED.show();

    frame_time_start_millis = current_mills_cached;
  }
}

void cacheLeftColor(uint8_t r, uint8_t g, uint8_t b)
{
  cached_colors.color_left_flash = CRGB(r, g, b);
}

void cacheRightColor(uint8_t r, uint8_t g, uint8_t b)
{
  cached_colors.color_right_flash = CRGB(r, g, b);
}

void handleEvent()
{
  if (current_event->event_type == SETUP_EVENTS)
  {
    switch (current_event->setup_name)
    {
    case SetupEvents::Error:
      break;
    case SetupEvents::First_Song_Event:
      break;
    case SetupEvents::Turn_Off_Lights:
      support_array(0, LEDEND - 1) = CRGB::Black;
      reset_controllers();
      break;
    case SetupEvents::Left_Color:
      cacheLeftColor(current_event->color.r,
                     current_event->color.g,
                     current_event->color.b);
      break;
    case SetupEvents::Right_Color:
      cacheRightColor(current_event->color.r,
                      current_event->color.g,
                      current_event->color.b);
      break;
    case SetupEvents::Chroma_Event:
      if (current_event->event_value == 1)
      {
        IsChroma = true;
      }
      else if (current_event->event_value == 0)
      {
        IsChroma = false;
      }
      break;
    default:
      break;
    }
  }
  else if (current_event->event_type == SHOW_EVENTS)
  {
    switch (current_event->show_name)
    {
    case ShowEvents::Light_Off:
    break;

    case ShowEvents::Right_Color_On:
    break;

    case ShowEvents::Right_Color_Flash:
    break;

    case ShowEvents::Right_Color_Fade:
    break;

    case ShowEvents::Left_Color_On:
    break;

    case ShowEvents::Left_Color_Flash:
    break;

    case ShowEvents::Left_Color_Fade:
    break;

    case ShowEvents::Left_Laser_Speed:
          leftLaser.laserSpeed = event_value;
      laser_left_time_update = 151 / leftLaser.laserSpeed;

    break;

    case ShowEvents::Right_Laser_Speed:
          rightLaser.laserSpeed = event_value;
      laser_right_time_update = 151 / rightLaser.laserSpeed;

    break;
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
    default:
      break;
    }
  }
}

void controlLight()
{

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
