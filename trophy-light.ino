#include <FastLED.h>

#define NUM__BODY_LEDS 10
#define NUM__BALL_LEDS 4
#define NUM__SHIELD_LEDS 6

#define TIMES_PER_SECOND(x) EVERY_N_MILLISECONDS(1000/x)
#define ARRAYLENGTH(x) (sizeof(x)/sizeof(x[0]))

CRGB bodyLeds[NUM__BODY_LEDS];
CRGB ballLeds[NUM__BALL_LEDS];
CRGB shieldLeds[NUM__SHIELD_LEDS];

CHSV hsv_bodyLeds[NUM__BODY_LEDS];
CHSV hsv_ballLeds[NUM__BALL_LEDS];
CHSV hsv_shieldLeds[NUM__SHIELD_LEDS];

int step = 50;
int maxValue = 200;

int g_brightness = 255;

int loopCounter = 0;
int maxNumberOfLoops = NUM__BODY_LEDS + maxValue / step;
int chargeUpIter = 1;

bool charging = true;

CHSV blue = CHSV(160, 255, 0);
CHSV orange = CHSV(25, 255, 0);

void setup() {
    //Body strips
    // FastLED.addLeds<NEOPIXEL, 33>(bodyLeds, NUM__BODY_LEDS);
    // FastLED.addLeds<NEOPIXEL, 32>(bodyLeds, NUM__BODY_LEDS);
    // FastLED.addLeds<NEOPIXEL, 31>(bodyLeds, NUM__BODY_LEDS);

    // //Ball strips
    // FastLED.addLeds<NEOPIXEL, 34>(ballLeds, NUM__BODY_LEDS);

    // //Shild strips
    // FastLED.addLeds<NEOPIXEL, 35>(shieldLeds, NUM__BODY_LEDS);

    FastLED.setBrightness(g_brightness);

    reset(blue);
}

void reset(CHSV color)
{
    for(int i = 0; i < NUM__BODY_LEDS; ++i)
    {
        hsv_bodyLeds[i] = color;
    }
    for(int i = 0; i < NUM__BALL_LEDS; ++i)
    {
        hsv_ballLeds[i] = color;
    }
}

void chargeRowUntilIter(int iter)
{
    for(int i = 0; i < iter; ++i)
    {
        if(hsv_bodyLeds[i].v < maxValue)
        {
            hsv_bodyLeds[i].v += step;
        }
    }
}

void chargePattern(bool shouldSwapColor)
{
    chargeRowUntilIter(chargeUpIter);

    if(chargeUpIter < NUM__BODY_LEDS-1)
    {
        ++chargeUpIter;
    }

    ++loopCounter;

    if(loopCounter == maxNumberOfLoops)
    {
        loopCounter = 0;
        chargeUpIter = 0;
        step *= -1;
        charging = !charging;
        if(shouldSwapColor)
        {
            swapColor();
        }
    }

}

void swapColor() {
    static int diff = -135;
    for(int i = 0; i < NUM__BODY_LEDS; ++i)
    {
        hsv_bodyLeds[i].hue += diff;
    }

    for(int i = 0; i < NUM__BALL_LEDS; ++i)
    {
        hsv_ballLeds[i].hue += diff;
    }

    diff *= -1;
}

void chargeBall(int startPoint)
{
    if(loopCounter < startPoint || hsv_ballLeds[0].v > maxValue)
    {
        return;
    }

    for(int i = 0; i < NUM__BALL_LEDS; ++i)
    {
        hsv_ballLeds[i].v += step;
    }
}

void copyToCRGB()
{
    for(int i=0; i<NUM__BODY_LEDS; i++){
        bodyLeds[i] = CRGB(hsv_bodyLeds[i]);
    }
    for(int i=0; i<NUM__BALL_LEDS; i++){
        ballLeds[i] = CRGB(hsv_ballLeds[i]);
    }
    for(int i=0; i<NUM__SHIELD_LEDS; i++){
        shieldLeds[i] = CRGB(hsv_shieldLeds[i]);
    }
}

void loop() {
    while(true) {
        if(charging)
        {
            chargePattern(false);
            delay(50);
            chargeBall(10);
        } else
        {
            chargePattern(true);
            delay(20);
            chargeBall(6);
        }

        copyToCRGB();
        FastLED.show();
    }
}
