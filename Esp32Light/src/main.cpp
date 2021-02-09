#include "main.hpp"

// global vars
t_cached_colors cached_colors;

bool chromaMap = false;

CRGB leds[TOTAL_LEDS];
CRGBArray<TOTAL_LEDS> support_array;

size_t total_controllers = 7;
LightController *stripeControllers[7];
LaserController *laserControllers[2];

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

    cached_colors.left = defaultColorLEFT;
    cached_colors.right = defaultColorRIGHT;

    init_controllers();

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
    for (size_t i = 0; i < total_controllers; ++i)
    {
        stripeControllers[i]->reset_controller();
    }
}

void init_controllers()
{
    size_t valuesMinMax[] = {0, 0, 0, 0, 0, 0, 0, 0};

    calculate_led_splits(valuesMinMax);

    stripeControllers[0] = new LightController(&support_array, valuesMinMax[0], valuesMinMax[1]);

    stripeControllers[1] = new LaserController(&support_array, valuesMinMax[1], valuesMinMax[2], true, 1);
    laserControllers[0] = (LaserController *)stripeControllers[1];

    stripeControllers[2] = new LightController(&support_array, valuesMinMax[2], valuesMinMax[3]);
    stripeControllers[3] = new LightController(&support_array, valuesMinMax[3], valuesMinMax[4]);
    stripeControllers[4] = new LightController(&support_array, valuesMinMax[4], valuesMinMax[5]);

    stripeControllers[5] = new LaserController(&support_array, valuesMinMax[5], valuesMinMax[6], false, 1);
    laserControllers[1] = (LaserController *)stripeControllers[5];

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
        if (i == 1 || i == 5)
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
        update_leds(leds, support_array);

        // Show updates
        FastLED.show();

        frame_time_start_millis = current_mills_cached;
    }
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
    else if (current_event->event_type == SHOW_EVENTS)
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
            laserControllers[0]->set_speed(current_event->event_value, current_mills_cached);
            break;
        case ShowEvents::Right_Laser_Speed:
            laserControllers[1]->set_speed(current_event->event_value, current_mills_cached);
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
