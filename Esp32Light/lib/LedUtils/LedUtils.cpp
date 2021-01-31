#include "LedUtils.hpp"

void update_leds(CRGB *actual_leds, CRGB *support_array, int from, int to)
{
    for (int i = from; i < to; ++i)
    {
        actual_leds[i] = support_array[i];
    }
}

// Checks leds by turning all them on with colors RED -> GREEN -> BLUE
void checkLeds(CRGB *actual_leds, int from, int to)
{
    //Led check
    for (int i = from; i < to; i++)
    {
        actual_leds[i] = CRGB::Black;
    }

    FastLED.show();

    for (int i = from; i < to; i++)
    {
        actual_leds[i] = CRGB::Red;
    }
    FastLED.show();
    delay(500);

    for (int i = from; i < to; i++)
    {
        actual_leds[i] = CRGB::Green;
    }
    FastLED.show();
    delay(500);

    for (int i = from; i < to; i++)
    {
        actual_leds[i] = CRGB::Blue;
    }
    FastLED.show();
    delay(500);

    for (int i = from; i < to; i++)
    {
        actual_leds[i] = CRGB::Black;
    }
    FastLED.show();
}

void resetModeDisplay()
{
    digitalWrite(PIN_LED_0, LOW);
    digitalWrite(PIN_LED_1, LOW);
    digitalWrite(PIN_LED_2, LOW);
    digitalWrite(PIN_LED_3, LOW);
    digitalWrite(PIN_LED_4, LOW);
}

void turnOnModeDisplay(size_t n)
{
    resetModeDisplay();
    if (n & 1)
    {
        digitalWrite(PIN_LED_0, HIGH);
    }
    if (n & 2)
    {
        digitalWrite(PIN_LED_1, HIGH);
    }
    if (n & 4)
    {
        digitalWrite(PIN_LED_2, HIGH);
    }
    if (n & 8)
    {
        digitalWrite(PIN_LED_3, HIGH);
    }
    if (n & 16)
    {
        digitalWrite(PIN_LED_4, HIGH);
    }
}