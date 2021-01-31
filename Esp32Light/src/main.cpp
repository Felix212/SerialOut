#include "main.hpp"

// global vars
t_cached_colors cached_colors;

bool IsChroma = false;

CRGB leds[NUM_LEDS];
CRGBArray<NUM_LEDS> support_array;

t_light_controller stripeControl[7];

t_laser leftLaser;
t_laser rightLaser;

LightToSerialParser *serialParser;

uint32_t current_mills_cached;

uint32_t frame_time_start_millis;
uint32_t fade_time_start;

uint32_t laser_left_timer_start;
uint32_t laser_right_timer_start;
uint32_t laser_left_time_update;
uint32_t laser_right_time_update;

//light events control
CRGB chromaColor; // defined outside to not declare every event
LightEvents current_event;
t_light_controller *current_handler;

void init_controllers()
{
  int valuesMinMax[] = {LEDSTART, LEDSPLIT1, LEDSPLIT2, LEDSPLIT3, LEDSPLIT4, LEDSPLIT5, LEDSPLIT6, LEDEND};
  for (int i = 0; i < 7; i++)
  {
    stripeControl[i].from = valuesMinMax[i];
    stripeControl[i].to = valuesMinMax[i + 1];
    stripeControl[i].status.ON = 0;
    stripeControl[i].status.FADE = 0;
    stripeControl[i].status.FLASH = 0;
    stripeControl[i].color = CRGB::Black;
    stripeControl[i].color_flash = CRGB::Black;
    stripeControl[i].actual_color = CRGB::Black;
  }
}

void setup()
{

  pinMode(PIN_LED_0, OUTPUT);
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);
  pinMode(PIN_LED_4, OUTPUT);

  cacheColors(&defaultColorLEFT, &defaultColorRIGHT);

  Serial.begin(BAUD_RATE);
  serialParser = new LightToSerialParser();

  init_controllers();

  // FastLED.addLeds<LEDTYPE, DATA_PIN, GRB>(leds, NUM_LEDS);

  FastLED.addLeds<LEDTYPE, DATA_PIN>(leds, NUM_LEDS);

  FastLED.setBrightness(BRIGHTNESS);

  if (POWERLIMIT != -1)
  {
    FastLED.setMaxPowerInMilliWatts(POWERLIMIT);
  }

  leftLaser.strip_part_index = 1;
  rightLaser.strip_part_index = 5;

  checkLeds(leds, 0, NUM_LEDS);

  frame_time_start_millis = 0;
  fade_time_start = 0;
  laser_left_timer_start = 0;
  laser_right_timer_start = 0;

  laser_left_time_update = 151 / leftLaser.laserSpeed;
  laser_right_time_update = 151 / rightLaser.laserSpeed;
}

void cacheColors(CRGB *left, CRGB *right)
{
  cached_colors.color_left_flash = *left;
  cached_colors.color_right_flash = *right;

  cached_colors.color_left = CRGB((uint8_t)((float)(*left).r * BRIGHTNESSDIVIDER),
                                  (uint8_t)((float)(*left).g * BRIGHTNESSDIVIDER),
                                  (uint8_t)((float)(*left).b * BRIGHTNESSDIVIDER));

  cached_colors.color_right = CRGB((uint8_t)((float)(*right).r * BRIGHTNESSDIVIDER),
                                   (uint8_t)((float)(*right).g * BRIGHTNESSDIVIDER),
                                   (uint8_t)((float)(*right).b * BRIGHTNESSDIVIDER));
}

void show_frame()
{
  if (current_mills_cached - frame_time_start_millis > TIME_BETWEEN_UPDATES)
  {
    // Serial.println("Showing frame");
    // update leds with current settings
    update_leds(leds, support_array, 0, NUM_LEDS);

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
        fadeFlashLight(i);
      }
      else if (stripeControl[i].status.FADE == 1)
      {
        fadeLight(i);
      }
    }
    fade_time_start = current_mills_cached;
  }
}

void loop()
{
  current_mills_cached = millis();

  digitalWrite(PIN_LED_4, HIGH);
  while (serialParser->dataAvailable()) // Only do something if there's new data
  {
    serialParser->readData();
    handleEvent();
  }
  digitalWrite(PIN_LED_4, LOW);

  fade_leds();

  //laser walkanimation
  ledwalkleft(&leftLaser, stripeControl[leftLaser.strip_part_index].from, stripeControl[leftLaser.strip_part_index].to);
  ledwalkright(&rightLaser, stripeControl[rightLaser.strip_part_index].from, stripeControl[rightLaser.strip_part_index].to);

  // coloring actual leds
  show_frame();
}

void handleChromaEvent(byte val)
{
  if (val == 1)
  {
    IsChroma = true;
    if (DEBUG_MESSAGES)
    {
      Serial.println(F("chroma enabled"));
    }

    else if (val == 0)
    {
      IsChroma = false;
      if (DEBUG_MESSAGES)
      {
        Serial.println(F("chroma disabled"));
      }
    }
  }
}

void handleEvent()
{
  t_lightEvent *event = serialParser->parseMessage();

  // stripe is split into 7 different sections. Change sections depending on eventtype
  switch (event->type)
  {
  case BACKTOPLASER:
    controlLight(3, event);
    break;
  case RINGLASER:
    controlLight(0, event);
    controlLight(6, event);
    break;
  case LEFTLASER:
    controlLight(1, event);
    break;
  case RIGHTLASER:
    controlLight(5, event);
    break;
  case CENTERLIGHT:
    controlLight(2, event);
    controlLight(4, event);
    break;

  // Configuration Events
  case LEFTLASERSPEED:
    leftLaser.laserSpeed = event->value;
    laser_left_time_update = 151 / leftLaser.laserSpeed;
    break;
  case RIGHTLASERSPEED:
    rightLaser.laserSpeed = event->value;
    laser_right_time_update = 151 / rightLaser.laserSpeed;
    break;
  case TURNOFFLIGHTS:
    support_array(0, LEDEND - 1) = CRGB::Black;
    break;
  case LEFTCOLOR:
    cacheColors(&event->color, &cached_colors.color_right_flash);
    break;
  case RIGHTCOLOR:
    cacheColors(&cached_colors.color_left_flash, &event->color);
    break;
  case CHROMAEVENT:
    handleChromaEvent(event->value);
    break;
  default:
    break;
  }
  // event handled
  free(event);
}

void controlLight(int index, t_lightEvent *event)
{
  current_handler = &stripeControl[index];
  current_event = (LightEvents)event->value;

  if (DEBUG_MESSAGES)
  {
    Serial.println(F("Handling light"));
    Serial.println(F("Message Received:"));
    Serial.print(F("Type: "));
    Serial.println(event->type);
    Serial.print(F("Value: "));
    Serial.println(event->value);
    Serial.print(F("Color: "));
    Serial.println("(" + String(event->color.r) + "," + String(event->color.g) + "," + String(event->color.b) + ")");
    Serial.print(F("Current light color: "));
    Serial.println("(" + String(current_handler->color.r) + "," + String(current_handler->color.g) + "," + String(current_handler->color.b) + ")");
  }

  //handle chroma
  if (IsChroma)
  {
    chromaColor = CRGB(event->color.r,
                       event->color.g,
                       event->color.b);
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
    if (event->value > 0 && event->value < 4)
    {
      current_handler->color_flash = cached_colors.color_right_flash;
      current_handler->color = cached_colors.color_right;
    }

    if (event->value > 4 && event->value < 8)
    {
      current_handler->color_flash = cached_colors.color_left_flash;
      current_handler->color = cached_colors.color_left;
    }
  }

  switch (current_event)
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
      if (index != 1 && index != 5) // if not laser
      {
        support_array(current_handler->from, current_handler->to - 1) = current_handler->color_flash;
      }
      current_handler->actual_color = current_handler->color_flash;
    }
    else
    {
      if (index != 1 && index != 5) // if not laser
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
void fadeLight(int index)
{
  current_handler = &stripeControl[index];
  current_handler->actual_color = current_handler->actual_color.fadeToBlackBy(1);

  if (index != 1 && index != 5) // if not laser
  {
    support_array(current_handler->from, current_handler->to - 1) = current_handler->actual_color;
  }

  if (current_handler->actual_color.getAverageLight() < 10)
  {
    support_array(current_handler->from, current_handler->to - 1) = CRGB::Black;
    current_handler->actual_color = CRGB::Black;
    current_handler->status.FADE = 0;
    current_handler->status.ON = 0;
  }
}

// fade until rgb value of  is reached
void fadeFlashLight(int index)
{
  current_handler = &stripeControl[index];
  current_handler->actual_color = current_handler->actual_color.fadeToBlackBy(1);
  if (index != 1 && index != 5) // if not laser
  {
    support_array(current_handler->from, current_handler->to - 1) = current_handler->actual_color;
  }

  if (current_handler->actual_color.getLuma() <= current_handler->color.getLuma())
  {
    if (index != 1 && index != 5) // if not laser
    {
      support_array(current_handler->from, current_handler->to - 1) = current_handler->color;
    }
    current_handler->actual_color = current_handler->color;
    current_handler->status.FLASH = 0;
  }
}

void ledwalkleft(struct Laser *laser, int from, int to)
{

  if (stripeControl[laser->strip_part_index].status.ON == 0)
  {
    return;
  }

  if (from + laser->laserIndex == to)
  {
    laser->laserIndex = 0;
    laser->toggle = !laser->toggle;
  }

  if (current_mills_cached - laser_left_timer_start > laser_left_time_update)
  {
    if (laser->toggle)
    {
      support_array[from + laser->laserIndex] = CRGB::Black;
    }
    else
    {
      support_array[from + laser->laserIndex] = stripeControl[laser->strip_part_index].actual_color;
    }

    laser->laserIndex++;
    laser_left_timer_start = current_mills_cached;
  }
}

void ledwalkright(struct Laser *laser, int from, int to)
{
  if (stripeControl[laser->strip_part_index].status.ON == 0)
  {
    return;
  }

  if (to - laser->laserIndex - 1 == from)
  {
    laser->laserIndex = 0;
    laser->toggle = !laser->toggle;
  }

  if (current_mills_cached - laser_right_timer_start > laser_right_time_update)
  {
    if (laser->toggle)
    {
      support_array[to - laser->laserIndex - 1] = CRGB::Black;
    }
    else
    {
      support_array[to - laser->laserIndex - 1] = stripeControl[laser->strip_part_index].actual_color;
    }

    laser->laserIndex++;
    laser_right_timer_start = current_mills_cached;
  }
}
