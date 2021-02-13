
#include <stdint.h>
#include "FastLED.h"

// -------------------------------------------------- Leds Strips --------------------------------------------------- //
/*
If you need to increase or decrease number of led strips, you also need to update the adding function "addLedsToFastLEDS"
Since you can connect multiple strips, two array of leds will be used. One represents leds updated by light controllers
and one the actual leds connected to board. The function "update_leds" in Utils.hpp copies the support array to the true
leds array. If a certain part of leds has flag "reverse" setted to true, then it will be copied in reverse to actual leds.
For example:
1 2 3 4 5 6 7 8 9 10
1 2 3 4 5 1 2 3 4 5

1 2 3 4 5 6 7 8 9 10
5 4 3 2 1 1 2 3 4 5

In the second case, the first part flag is true.
This is needed since leds strips count leds starting from connection to board, so if you place them one near the other they
have to be reversed.

If you have any questions, contact Kratos#0901 on discord.
*/
#define PIN_LED_STRIP_1 53
#define PIN_LED_STRIP_2 52
#define N_LEDS_STRIPS   {108,  120  }
#define REVERSE_DISPLAY {true, false}

#define TOTAL_STRIPS 2

// Total Leds to use
#define TOTAL_LEDS 228

// ------------------------------------------------------ Leds ------------------------------------------------------ //
// If LEDTYPE is NEOPIXEL it is necessary to edit file Utils.cpp in function addLedsToFastLED at line
// FastLED.addLeds<LEDTYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
// with
// FastLED.addLeds<LEDTYPE, DATA_PIN>(leds, NUM_LEDS);
#define LEDTYPE NEOPIXEL

// Max usage of power in milliwatts through the board. If external power, not from board, is provided to Leds, 
// it can be -1, so unlimited
#define POWERLIMIT -1

// Has to be same that in game
#define BAUD_RATE 250000

// How many times every seconds update Leds
#define UPDATES_PER_SECOND 90

// --------------------------------------------------- Split parts --------------------------------------------------- //
// A minimum of 7 leds is required
// Splits will be defined as [From - To) meaning FROM index will be included and TO index not
// If a split is 4-10, then used indices will be 4-5-6-7-8-9

// If custom splits is not enabled, then splits will all be same length according to TOTAL_LEDS
// Enable: 1 or true
// Disable: 0 or false
#define CUSTOM_SPLITS 0

// A full stripe will be divided in 7 parts
// RingLaserLeft - LeftLaser - CenterLightLeft - BackTopLaser - CenterLightRight - RightLaser - RingLaserRight

#define LEDSTART 10
#define LEDSPLIT1 34  // RingLaser # LEDSTART --- LEDSPLIT1
#define LEDSPLIT2 66  // LeftLaser # LEDSPLIT1 --- LEDSPLIT2
#define LEDSPLIT3 98  // Centerlight # etc.
#define LEDSPLIT4 130 // Backtoplight
#define LEDSPLIT5 162 // Centerlight
#define LEDSPLIT6 194 // RightLaser
#define LEDEND 228    // RingLaser

// --------------------------------------------------- End Setup ---------------------------------------------------- //
// Default Colors
#define DEFAULT_LEFT_COLOR CRGB(255, 0, 0)
#define DEFAULT_RIGHT_COLOR CRGB(0, 125, 255)

// time between every fade update to led. Every update light will be faded to black by 1
#define FADE_TIME_MILLIS 20

// time between every fade update to led. Every update light will be faded to normal color by 1
#define FLASH_TIME_MILLIS 20

// When a command to turn on a light comes, minumim of this milliseconds that light will be on even if a event to turn off comes
#define MINIUM_TURN_ON_LIGHT_TIME 30

// Flash events are 60% brighter than normal light on events
#define BRIGHTNESSDIVIDER 0.4f
// If you power your leds by the ESP32 directly, dont use anything higher than 130. 255 is maximum.
#define BRIGHTNESS_MAX 255

// ----------------------------------------------------- Events ----------------------------------------------------- //
// Show events
#define LIGHT_OFF 0

#define RIGHT_COLOR_ON 1
#define RIGHT_COLOR_FLASH 2
#define RIGHT_COLOR_FADE 3

#define LEFT_COLOR_ON 5
#define LEFT_COLOR_FLASH 6
#define LEFT_COLOR_FADE 7

#define LEFTLASERSPEED 12
#define RIGHTLASERSPEED 13

// from 240 there are setup events
#define SETUPEVENTS 251
#define TURNOFFLIGHTS 252

#define LEFTCOLOR 253
#define RIGHTCOLOR 254

#define CHROMAEVENT 255

// Light Groups
#define BACKTOPLASER 0
#define RINGLASER 1
#define LEFTLASER 2
#define RIGHTLASER 3
#define CENTERLIGHT 4

// ----------------------------------------------- Support Structures ----------------------------------------------- //

enum class EventsType
{
    Show,
    Setup
};

enum class LightGroup
{
    Error,
    BackTopLaser,
    RingLaser,
    LeftLaser,
    RightLaser,
    CenterLight
};

enum class SetupEvents
{
    Error,
    First_Song_Event,
    Turn_Off_Lights,
    Left_Color,
    Right_Color,
    Chroma_Event
};

enum class ShowEvents
{
    Error,
    Light_Off,
    Right_Color_On,
    Right_Color_Flash,
    Right_Color_Fade,
    Left_Color_On,
    Left_Color_Flash,
    Left_Color_Fade,
    Left_Laser_Speed,
    Right_Laser_Speed,
};

typedef struct StatusLight
{
    bool fade;
    bool flash;
    bool on;
} t_status;

typedef struct CacheColors
{
    CRGB left;
    CRGB right;
} t_cached_colors;

typedef struct Event
{
    EventsType event_type;
    SetupEvents setup_name;
    ShowEvents show_name;
    LightGroup light_group;
    int event_value;
    CRGB color;
} t_lightEvent;

// ----------------------------------------------------- Utils ------------------------------------------------------ //
void addLedsToFastLEDS(CRGB *leds)
{
    int n_led_strips[] = N_LEDS_STRIPS;
    int start_index = 0;

    // FastLED.addLeds<LEDTYPE, 1, GRB>(leds, start_index, n_led_strips[i]);
    FastLED.addLeds<LEDTYPE, PIN_LED_STRIP_1>(leds, start_index, n_led_strips[0]);
    start_index += n_led_strips[0];
    FastLED.addLeds<LEDTYPE, PIN_LED_STRIP_2>(leds, start_index, n_led_strips[1]);
}

void calculate_led_splits(size_t *parts)
{
    if (CUSTOM_SPLITS)
    {
        parts[0] = LEDSTART;
        parts[1] = LEDSPLIT1;
        parts[2] = LEDSPLIT2;
        parts[3] = LEDSPLIT3;
        parts[4] = LEDSPLIT4;
        parts[5] = LEDSPLIT5;
        parts[6] = LEDSPLIT6;
        parts[7] = LEDEND;
    }
    else
    {
        int increment = TOTAL_LEDS / 7;
        int missing_leds = TOTAL_LEDS - increment * 7;

        for (size_t i = 1; i < 8; ++i)
        {
            parts[i] = parts[i - 1] + increment;
        }

        switch (missing_leds)
        {
        // BackTopLaser
        case 1:
            parts[1] += 0;
            parts[2] += 0;
            parts[3] += 0;
            parts[4] += 1;
            parts[5] += 1;
            parts[6] += 1;
            parts[7] += 1;
            break;
        // Both Lasers
        case 2:
            parts[1] += 0;
            parts[2] += 1;
            parts[3] += 1;
            parts[4] += 1;
            parts[5] += 1;
            parts[6] += 2;
            parts[7] += 2;
            break;
        // Both Lasers - BackTopLaser
        case 3:
            parts[1] += 0;
            parts[2] += 1;
            parts[3] += 1;
            parts[4] += 2;
            parts[5] += 2;
            parts[6] += 3;
            parts[7] += 3;
            break;
        case 4:
            // Both Lasers - Both Center Lights
            parts[1] += 0;
            parts[2] += 1;
            parts[3] += 2;
            parts[4] += 2;
            parts[5] += 3;
            parts[6] += 4;
            parts[7] += 4;
            break;
        // Both Lasers - Both Center Lights - BackTopLaser
        case 5:
            parts[1] += 0;
            parts[2] += 1;
            parts[3] += 2;
            parts[4] += 3;
            parts[5] += 4;
            parts[6] += 5;
            parts[7] += 5;
            break;
        // Both Lasers - Both Center Lights - Both RingLights
        case 6:
            parts[1] += 1;
            parts[2] += 2;
            parts[3] += 3;
            parts[4] += 3;
            parts[5] += 4;
            parts[6] += 5;
            parts[7] += 6;
            break;

        default:
            break;
        }
    }
}

// Checks leds by turning all them on with colors RED -> GREEN -> BLUE
void checkLeds(CRGB *actual_leds, int from, int to)
{
    //Led check
    for (int i = from; i < to; ++i)
    {
        actual_leds[i] = CRGB::Black;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; ++i)
    {
        actual_leds[i] = CRGB::Red;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; ++i)
    {
        actual_leds[i] = CRGB::Green;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; ++i)
    {
        actual_leds[i] = CRGB::Blue;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; ++i)
    {
        actual_leds[i] = CRGB::Black;
    }

    FastLED.show();
}

void update_leds(CRGB *actual_leds, CRGB *support_array, int *n_leds_strips, bool *display_reverse)
{
    int i, j, k = 0;
    int total_placed_leds = 0;

    for (i = 0; i < TOTAL_STRIPS; i++)
    {
        if (display_reverse[i])
        {
            for (j = n_leds_strips[i] - 1; j >= total_placed_leds; --j)
            {
                actual_leds[j] = support_array[k];
                k = k + 1;
            }
        }
        else
        {
            for (j = total_placed_leds; j < total_placed_leds + n_leds_strips[i]; ++j)
            {
                actual_leds[j] = support_array[k];
                k = k + 1;
            }
        }

        total_placed_leds += n_leds_strips[i];
    }
}

void byteToRGB(CRGB *color, byte data_color)
{
    color->r = (data_color >> 5) * 32;
    color->g = ((data_color & 28) >> 2) * 32;
    color->b = (data_color & 3) * 64;
}

bool sameColor(CRGB *c1, CRGB *c2)
{
    return c1->r == c2->r && c1->g == c2->g && c1->b == c2->b;
}

LightGroup valToLightGroup(byte val)
{
    switch (val)
    {
    case BACKTOPLASER:
        return LightGroup::BackTopLaser;
    case RINGLASER:
        return LightGroup::RingLaser;
    case LEFTLASER:
        return LightGroup::LeftLaser;
    case RIGHTLASER:
        return LightGroup::RightLaser;
    case CENTERLIGHT:
        return LightGroup::CenterLight;
    default:
        return LightGroup::Error;
    }
}

SetupEvents nameToSetupEvent(byte name)
{
    switch (name)
    {
    case SETUPEVENTS:
        return SetupEvents::First_Song_Event;
    case TURNOFFLIGHTS:
        return SetupEvents::Turn_Off_Lights;
    case LEFTCOLOR:
        return SetupEvents::Left_Color;
    case RIGHTCOLOR:
        return SetupEvents::Right_Color;
    case CHROMAEVENT:
        return SetupEvents::Chroma_Event;
    default:
        return SetupEvents::Error;
    }
}

ShowEvents nameToShowEvent(byte name)
{
    switch (name)
    {
    case LIGHT_OFF:
        return ShowEvents::Light_Off;
    case RIGHT_COLOR_ON:
        return ShowEvents::Right_Color_On;
    case RIGHT_COLOR_FADE:
        return ShowEvents::Right_Color_Fade;
    case RIGHT_COLOR_FLASH:
        return ShowEvents::Right_Color_Flash;
    case LEFT_COLOR_ON:
        return ShowEvents::Left_Color_On;
    case LEFT_COLOR_FADE:
        return ShowEvents::Left_Color_Fade;
    case LEFT_COLOR_FLASH:
        return ShowEvents::Left_Color_Flash;
    case LEFTLASERSPEED:
        return ShowEvents::Left_Laser_Speed;
    case RIGHTLASERSPEED:
        return ShowEvents::Right_Laser_Speed;
    default:
        return ShowEvents::Error;
    }
}

// ------------------------------------------------ Light Controller ------------------------------------------------ //

class LightController
{
protected:
    CRGBSet *support_array;
    // in which status is the light
    t_status status;

    // begin led index
    int from;
    // to -1 is last led index of this light controller
    int to;

    CRGB color;
    CRGB color_flash;
    CRGB actual_color;
    bool color_changed;

    // time of when light was turn on
    uint32_t status_start_time;
    uint32_t last_fade_time;
    uint32_t current_time;

    // flag to turn of light after minimum time passed
    bool have_to_turn_off;

    void flashLight();
    void fadeLight();
    void update_color(CRGB);
    void update_status(t_status, uint32_t);
    void reset_colors();
    void reset_status();
    void turn_off_leds();
    void compute_actual_color();

public:
    LightController(CRGBSet *, int, int);
    ~LightController();
    void update(uint32_t);
    void handle_event(t_status, CRGB, uint32_t);
    void reset_controller();
};

// -------------------------------------------------- Constructors -------------------------------------------------- //
LightController::LightController(CRGBSet *leds, int from, int to)
{
    this->support_array = leds;
    this->from = from;
    this->to = to;
    this->reset_colors();
    this->reset_status();
}

LightController::~LightController()
{
}

// ------------------------------------------------ Protected Methods ------------------------------------------------- //
void LightController::update_color(CRGB flash_color)
{
    if (sameColor(&flash_color, &this->color_flash))
    {
        return;
    }

    this->color_changed = true;

    this->color_flash.setRGB(flash_color.r,
                             flash_color.g,
                             flash_color.b);
    this->color.setRGB((uint8_t)((float)flash_color.r * BRIGHTNESSDIVIDER),
                       (uint8_t)((float)flash_color.g * BRIGHTNESSDIVIDER),
                       (uint8_t)((float)flash_color.b * BRIGHTNESSDIVIDER));
}

void LightController::reset_colors()
{
    this->color_changed = true;
    this->color = CRGB::Black;
    this->color_flash = CRGB::Black;
    this->actual_color = CRGB::Black;
}

void LightController::reset_status()
{
    this->status_start_time = 0;
    this->status.fade = 0;
    this->status.flash = 0;
    this->status.on = 0;
    this->have_to_turn_off = false;
}

void LightController::update_status(t_status new_status, uint32_t time)
{
    this->status_start_time = time;
    this->last_fade_time = time;

    if (new_status.on)
    {
        // update current status
        this->status = new_status;
        this->have_to_turn_off = false;

        if (this->status.flash)
        {
            this->actual_color = this->color_flash;
        }
        else
        {
            this->actual_color = this->color;
        }
    }
    else
    {
        this->have_to_turn_off = true;
        this->status.fade = false;
        this->status.flash = false;
    }
}

void LightController::flashLight()
{
    this->actual_color = this->actual_color.fadeToBlackBy(1);
    this->color_changed = true;

    if (this->actual_color.getLuma() < this->color.getLuma())
    {
        this->actual_color = this->color;
        this->status.flash = false;
    }
}

void LightController::fadeLight()
{
    this->actual_color = this->actual_color.fadeToBlackBy(1);
    this->color_changed = true;
    if (this->actual_color.getAverageLight() < 16)
    {
        this->actual_color = CRGB::Black;
        reset_status();
    }
}

void LightController::compute_actual_color()
{
    if (this->have_to_turn_off)
    {
        if (this->current_time - this->status_start_time > MINIUM_TURN_ON_LIGHT_TIME)
        {
            this->have_to_turn_off = false;
            this->status.on = false;
            this->actual_color = CRGB::Black;
            this->color_changed = true;
        }
    }
    else if (this->status.fade)
    {
        if (this->current_time - this->last_fade_time > FADE_TIME_MILLIS)
        {
            this->fadeLight();
            this->last_fade_time = this->current_time;
        }
    }
    else if (this->status.flash)
    {
        if (this->current_time - this->last_fade_time > FLASH_TIME_MILLIS)
        {
            this->flashLight();
            this->last_fade_time = this->current_time;
        }
    }
}

void LightController::turn_off_leds()
{
    (*support_array)(this->from, this->to - 1) = CRGB::Black;
}

// ------------------------------------------------- Public Methods ------------------------------------------------- //
void LightController::handle_event(t_status new_status, CRGB new_color, uint32_t event_time)
{
    // If color is not black
    if (new_color.getAverageLight() > 0)
    {
        this->update_color(new_color);
    }
    this->update_status(new_status, event_time);
}

void LightController::reset_controller()
{
    this->reset_status();
    this->reset_colors();
    this->turn_off_leds();
}

void LightController::update(uint32_t current_millis)
{
    this->current_time = current_millis;

    if (this->status.on)
    {
        this->compute_actual_color();

        // Apply current color to support vector
        if (this->color_changed)
        {
            (*support_array)(this->from, this->to - 1) = this->actual_color;
        }
    }
}

// ------------------------------------------------ Laser Controller ------------------------------------------------ //

class LaserController : public LightController
{
protected:
    int speed;
    int current_offset;
    bool direction;
    bool can_shift;
    uint32_t last_laser_time;
    uint32_t time_between_laser_updates;
    void colorLeds();
    void updateOffset();

public:
    LaserController(CRGBSet *, int, int, bool, int);
    ~LaserController();

    // current time
    void update(uint32_t);

    // New status, Color to apply, current time
    void handle_event(t_status, CRGB, uint32_t);

    void reset_controller();
    void set_speed(int, uint32_t);
};

// -------------------------------------------------- Constructors -------------------------------------------------- //
LaserController::LaserController(CRGBSet *leds, int from, int to, bool direction, int speed) : LightController::LightController(leds, from, to)
{
    this->direction = direction;
    this->current_offset = 0;
    set_speed(speed, 0);
}

// ------------------------------------------------ Private Methods ------------------------------------------------- //
void LaserController::updateOffset()
{
    // Apply current color to support vector
    // true -> to right
    // 100100 -> 010010
    // false -> to left
    // 100100 -> 001001
    if (direction)
    {
        this->current_offset = (this->current_offset + 1) % 3;
    }
    else
    {
        this->current_offset = this->current_offset - 1;
        if (this->current_offset < 0)
        {
            this->current_offset = 2;
        }
    }
}

void LaserController::colorLeds()
{
    for (int i = this->from; i < this->to; ++i)
    {
        if ((i - this->from) % 3 == this->current_offset)
        {
            (*support_array)[i] = this->actual_color;
        }
        else
        {
            (*support_array)[i] = CRGB::Black;
        }
    }
}

// ------------------------------------------------- Public Methods ------------------------------------------------- //

void LaserController::set_speed(int new_speed, uint32_t time)
{
    this->last_laser_time = time;
    if (new_speed == 0)
    {
        this->current_offset = 0;
        this->can_shift = false;
    }
    else
    {
        time_between_laser_updates = 151 / new_speed;
        this->can_shift = true;
    }
}

void LaserController::update(uint32_t current_millis)
{
    this->current_time = current_millis;

    if (this->status.on)
    {
        this->compute_actual_color();

        if (this->current_time - this->last_laser_time > this->time_between_laser_updates)
        {
            if (this->can_shift)
            {
                this->updateOffset();
            }
            this->colorLeds();
            this->last_laser_time = this->current_time;
        }
        else if (this->color_changed)
        {
            this->colorLeds();
        }
    }
}

// --------------------------------------------- Light To Serial Parser --------------------------------------------- //

class LightToSerialParser
{

private:
    uint8_t buffer_size = 2;
    uint8_t buffer[2];
    // first part of event to handle
    uint8_t ev_0;
    // second part of event to handle
    uint8_t ev_1;

public:
    LightToSerialParser();
    void readData();
    bool moreEvents();
    t_lightEvent *parseMessage();
};

LightToSerialParser::LightToSerialParser()
{
    for (size_t i = 0; i < buffer_size; ++i)
    {
        buffer[i] = 0;
    }
}

// Checks if there are events to handle
bool LightToSerialParser::moreEvents()
{
    return Serial.available();
}

// Read buffer_size bytes from Serial and stores them in parser buffer
void LightToSerialParser::readData()
{
    Serial.readBytes(buffer, buffer_size);
}

// Decode data in buffer as defined in https://github.com/Felix212/SerialOut/wiki/Serial-Protocol
t_lightEvent *LightToSerialParser::parseMessage()
{
    t_lightEvent *message = (t_lightEvent *)malloc(sizeof(t_lightEvent));
    message->event_value = 0;

    ev_0 = buffer[0];
    ev_1 = buffer[1];

    // Ev_0 has bits are something like 1111xxxx
    if (ev_0 > 239)
    {
        message->event_type = EventsType::Setup;
        message->setup_name = nameToSetupEvent(ev_0);
        if (message->setup_name == SetupEvents::Left_Color ||
            message->setup_name == SetupEvents::Right_Color)
        {
            byteToRGB(&message->color, ev_1);
        }
        else
        {
            message->event_value = ev_1;
        }
    }
    // Ev_0 has bits are something like 11xxyyyy
    // So just 1100yyyy, 1101yyyy, 1110yyyy since 1111xxxx has already been handled
    else if (ev_0 > 191)
    {
        message->event_type = EventsType::Show;
        message->show_name = nameToShowEvent(ev_0 >> 4);
        message->event_value = ev_0 & 15;
    }
    // Ev_0 bits are something like xxxxyyyy
    // xxxx will be light group, yyyy light event to apply to that group
    // 0000, ..., 1011 group
    // 0000, ..., 1011 event
    else
    {
        message->event_type = EventsType::Show;
        message->light_group = valToLightGroup(ev_0 >> 4);
        message->show_name = nameToShowEvent(ev_0 & 15);
        byteToRGB(&message->color, ev_1);
    }

    return message;
}

// ------------------------------------------------------ Main ------------------------------------------------------ //


// global vars
t_cached_colors cached_colors;

bool chromaMap = false;

CRGB leds[TOTAL_LEDS];
CRGBArray<TOTAL_LEDS> support_array;

int n_leds_strips[] = N_LEDS_STRIPS;
bool reverse_display[] = REVERSE_DISPLAY;

size_t total_controllers = 7;
LightController *stripeControllers[7];

uint32_t current_mills_cached;
uint32_t frame_time_start_millis;
uint32_t time_between_frame_updates;

LightToSerialParser *parser;
t_lightEvent *current_event;
t_status event_status;
CRGB color_to_use;

void setup()
{
    time_between_frame_updates = 1000 / UPDATES_PER_SECOND;

    parser = new LightToSerialParser();

    cached_colors.left = DEFAULT_LEFT_COLOR;
    cached_colors.right = DEFAULT_RIGHT_COLOR;

    init_controllers();

    addLedsToFastLEDS(leds);

    if (BRIGHTNESS_MAX != 255)
    {
        FastLED.setBrightness(BRIGHTNESS_MAX);
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
    for (size_t i = 0; i < total_controllers; ++i)
    {
        stripeControllers[i]->reset_controller();
    }
}

void init_controllers()
{
    size_t valuesMinMax[] = {0, 0, 0, 0, 0, 0, 0, 0};

    calculate_led_splits(valuesMinMax);

    // Left Ring
    stripeControllers[0] = new LightController(&support_array, valuesMinMax[0], valuesMinMax[1]);

    // Left Laser
    stripeControllers[1] = new LaserController(&support_array, valuesMinMax[1], valuesMinMax[2], true, 1);

    // Left Center
    stripeControllers[2] = new LightController(&support_array, valuesMinMax[2], valuesMinMax[3]);

    // BackTop
    stripeControllers[3] = new LightController(&support_array, valuesMinMax[3], valuesMinMax[4]);

    // Right Center
    stripeControllers[4] = new LightController(&support_array, valuesMinMax[4], valuesMinMax[5]);

    // Right Laser
    stripeControllers[5] = new LaserController(&support_array, valuesMinMax[5], valuesMinMax[6], false, 1);

    // Right Ring
    stripeControllers[6] = new LightController(&support_array, valuesMinMax[6], valuesMinMax[7]);
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

    // Update Leds Controllers
    for (size_t i = 0; i < total_controllers; ++i)
    {
        if (i == 1 || i == 5) // lasers
        {
            ((LaserController *)stripeControllers[i])->update(current_mills_cached);
        }
        else
        {
            stripeControllers[i]->update(current_mills_cached);
        }
    }

    // coloring actual leds
    if (current_mills_cached - frame_time_start_millis > time_between_frame_updates)
    {
        // update leds with current settings
        update_leds(leds, support_array, n_leds_strips, reverse_display);

        // Show updates
        FastLED.show();

        frame_time_start_millis = current_mills_cached;
    }
}

void handleEvent()
{
    if (current_event->event_type == EventsType::Setup)
    {
        switch (current_event->setup_name)
        {
        case SetupEvents::Error:
            break;
        case SetupEvents::First_Song_Event:
            break;
        case SetupEvents::Turn_Off_Lights:
            reset_controllers();
            break;
        case SetupEvents::Left_Color:
            cached_colors.left = current_event->color;
            break;
        case SetupEvents::Right_Color:
            cached_colors.right = current_event->color;
            break;
        case SetupEvents::Chroma_Event:
            if (current_event->event_value == 1)
            {
                chromaMap = true;
            }
            else if (current_event->event_value == 0)
            {
                chromaMap = false;
            }
            break;
        default:
            break;
        }
    }
    else if (current_event->event_type == EventsType::Show)
    {
        // if chroma map
        if (chromaMap)
        {
            color_to_use = current_event->color;
        }
        // default colors
        else if (current_event->show_name == ShowEvents::Right_Color_Fade ||
                 current_event->show_name == ShowEvents::Right_Color_Flash ||
                 current_event->show_name == ShowEvents::Right_Color_On)
        {
            color_to_use = cached_colors.right;
        }
        else if (current_event->show_name == ShowEvents::Left_Color_Fade ||
                 current_event->show_name == ShowEvents::Left_Color_Flash ||
                 current_event->show_name == ShowEvents::Left_Color_On)
        {
            color_to_use = cached_colors.left;
        }
        else
        {
            color_to_use = CRGB::Black;
        }

        switch (current_event->show_name)
        {
        case ShowEvents::Light_Off:
            event_status.on = 0;
            event_status.flash = 0;
            event_status.fade = 0;
            applyEventToGroup();
            break;
        case ShowEvents::Right_Color_On:
        case ShowEvents::Left_Color_On:
            event_status.on = 1;
            event_status.fade = 0;
            event_status.flash = 0;
            applyEventToGroup();
            break;
        case ShowEvents::Right_Color_Flash:
        case ShowEvents::Left_Color_Flash:
            event_status.on = 1;
            event_status.fade = 0;
            event_status.flash = 1;
            applyEventToGroup();
            break;
        case ShowEvents::Right_Color_Fade:
        case ShowEvents::Left_Color_Fade:
            event_status.on = 1;
            event_status.fade = 1;
            event_status.flash = 0;
            applyEventToGroup();
            break;
        case ShowEvents::Left_Laser_Speed:
            ((LaserController *)stripeControllers[1])->set_speed(current_event->event_value, current_mills_cached);
            break;
        case ShowEvents::Right_Laser_Speed:
            ((LaserController *)stripeControllers[5])->set_speed(current_event->event_value, current_mills_cached);
            break;
        default:
            break;
        }
    }
}

void applyEventToGroup()
{
    switch (current_event->light_group)
    {
    case LightGroup::BackTopLaser:
        stripeControllers[3]->handle_event(event_status, color_to_use, current_mills_cached);
        break;
    case LightGroup::RingLaser:
        stripeControllers[0]->handle_event(event_status, color_to_use, current_mills_cached);
        stripeControllers[6]->handle_event(event_status, color_to_use, current_mills_cached);
        break;
    case LightGroup::LeftLaser:
        stripeControllers[1]->handle_event(event_status, color_to_use, current_mills_cached);
        break;
    case LightGroup::RightLaser:
        stripeControllers[5]->handle_event(event_status, color_to_use, current_mills_cached);
        break;
    case LightGroup::CenterLight:
        stripeControllers[2]->handle_event(event_status, color_to_use, current_mills_cached);
        stripeControllers[4]->handle_event(event_status, color_to_use, current_mills_cached);
        break;
    default:
        break;
    }
}
