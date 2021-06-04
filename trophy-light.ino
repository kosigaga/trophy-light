#include <FastLED.h>

#define NUM__BODY_LEDS 10
#define NUM__BALL_LEDS 4
#define NUM__SHIELD_LEDS 6

CRGB bodyLeds[NUM_LEDS];
CRGB ballLeds[NUM_LEDS];
CRGB shieldLeds[NUM_LEDS];

CRGB blue1 = CRGB(200, 200, 255);
CRGB blue2 = CRGB(150, 150, 255);
CRGB blue3 = CRGB(100, 100, 255);
CRGB blue4 = CRGB(50, 50, 255);

CRGB orange1 = CRGB(255, 200, 164);
CRGB orange2 = CRGB(255, 150, 82);
CRGB orange3 = CRGB(255, 100, 41);
CRGB orange4 = CRGB(255, 50, 20);

void setup() {
    //Body strips
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(bodyLeds, NUM__BODY_LEDS);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(bodyLeds, NUM__BODY_LEDS);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(bodyLeds, NUM__BODY_LEDS);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(bodyLeds, NUM__BODY_LEDS);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(bodyLeds, NUM__BODY_LEDS);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(bodyLeds, NUM__BODY_LEDS);

    //Ball strips
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(ballLeds, NUM__BODY_LEDS);

    //Shild strips
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(shieldLeds, NUM__BODY_LEDS);
}

void chargePattern() {

}

void loop() {

    if (state1)
    {
        chargePattern();
    } else if(state2)
    {
        pulsePattern();
    }

    delay(30);
}
