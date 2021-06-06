#include <FastLED.h>

#define NUM__BODY_LEDS 10
#define NUM__BALL_LEDS 4
#define NUM__SHIELD_LEDS 4

#define TIMES_PER_SECOND(x) EVERY_N_MILLISECONDS(1000/x)
#define ARRAYLENGTH(x) (sizeof(x)/sizeof(x[0]))

CRGB bodyLeds[NUM__BODY_LEDS];
CRGB ballLeds[NUM__BALL_LEDS];
CRGB shieldLeds[NUM__SHIELD_LEDS];

CHSV hsv_bodyLeds[NUM__BODY_LEDS];
CHSV hsv_ballLeds[NUM__BALL_LEDS];
CHSV hsv_shieldLeds[NUM__SHIELD_LEDS];

int step = 25;
int maxValue = 225;

int g_brightness = 18;

int loopCounter = 0;
int maxNumberOfLoops = NUM__BODY_LEDS + maxValue / step;
int chargeUpIter = 0;

bool charging = true;
bool pulse = true;

CHSV blue = CHSV(160, 255, 0);
CHSV orange = CHSV(25, 255, 0);

void setup() {
    //Body strips
    FastLED.addLeds<NEOPIXEL, 33>(bodyLeds, NUM__BODY_LEDS);
    FastLED.addLeds<NEOPIXEL, 32>(bodyLeds, NUM__BODY_LEDS);
    FastLED.addLeds<NEOPIXEL, 14>(bodyLeds, NUM__BODY_LEDS);

    //Ball strips
    FastLED.addLeds<NEOPIXEL, 27>(ballLeds, NUM__BALL_LEDS);

    //Shild strips
    FastLED.addLeds<NEOPIXEL, 25>(shieldLeds, NUM__SHIELD_LEDS);

    FastLED.setBrightness(g_brightness);
    FastLED.clear();
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
    for(int i = 0; i < NUM__SHIELD_LEDS; ++i)
    {
        hsv_shieldLeds[i] = color;
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


void chargePattern()
{
    chargeRowUntilIter(chargeUpIter);

    if(chargeUpIter < NUM__BODY_LEDS)
    {
        ++chargeUpIter;
    }

    ++loopCounter;

    if(loopCounter == maxNumberOfLoops)
    {
        loopCounter = 0;
        chargeUpIter = 0;
        charging = !charging;
    }
}

void disChargePattern()
{
    disChargeRowUntilIter(chargeUpIter);

    if(chargeUpIter < NUM__BODY_LEDS)
    {
        ++chargeUpIter;
    }

    ++loopCounter;

    if(loopCounter == maxNumberOfLoops)
    {
        loopCounter = 0;
        chargeUpIter = 0;
        charging = !charging;
        swapColor();
    }

}
void disChargeRowUntilIter(int iter)
{
    for(int i = 0; i < iter; ++i)
    {
        if(hsv_bodyLeds[i].v > 0)
        {
            hsv_bodyLeds[i].v -= step;
        }
    }
}

void swapColor() {
    static int diff = -125;
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
    if(loopCounter < startPoint || hsv_ballLeds[0].v >= maxValue)
    {
        return;
    }

    for(int i = 0; i < NUM__BALL_LEDS; ++i)
    {
        hsv_ballLeds[i].v += step;
    }
}

void disChargeBall(int startPoint)
{
    if(loopCounter < startPoint || hsv_ballLeds[0].v <= 0)
    {
        return;
    }

    for(int i = 0; i < NUM__BALL_LEDS; ++i)
    {
        hsv_ballLeds[i].v -= step;
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

        for (int index = 0; index < NUM__SHIELD_LEDS; index++) {
            hsv_shieldLeds[index] = CHSV(160, 255, beatsin16(5, 120, 255));
        }

        if(charging)
        {
            chargePattern();
            chargeBall(10);
            delay(100);
        } else
        {
            disChargePattern();
            disChargeBall(6);
            delay(40);
        }

        copyToCRGB();
        FastLED.show();
    }
}
