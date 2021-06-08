#include <FastLED.h>

#define NUM__LEDS 10
#define NUM__EX_LEDS 10

#define WAVE_SIZE 3

#define TIMES_PER_SECOND(x) EVERY_N_MILLISECONDS(1000/x)
#define ARRAYLENGTH(x) (sizeof(x)/sizeof(x[0]))

CRGB leds[NUM__LEDS];
CHSV hsv_leds[NUM__LEDS + WAVE_SIZE*2];

CRGB ex_leds[NUM__EX_LEDS];
CHSV hsv_exLeds[NUM__EX_LEDS];

CHSV green = CHSV(90, 255, 100);

int g_brightness = 128;

void setup() {
    FastLED.addLeds<NEOPIXEL, 33>(leds, NUM__LEDS);
    FastLED.addLeds<NEOPIXEL, 32>(ex_leds, NUM__LEDS);

    FastLED.setBrightness(g_brightness);
    FastLED.clear();
    reset(green);
}

void reset(CHSV color)
{
    for(int i = 0; i < NUM__LEDS + WAVE_SIZE; ++i)
    {
        hsv_leds[i] = color;
    }

    for(int i = 0; i < NUM__EX_LEDS; ++i)
    {
        hsv_exLeds[i] = color;
    }
}

void copyToCRGB()
{
    for(int i = 0; i < NUM__LEDS; i++)
    {
        leds[i] = CRGB(hsv_leds[i + WAVE_SIZE]);
    }
    for(int i=0; i < NUM__EX_LEDS; i++)
    {
        ex_leds[i] = CRGB(hsv_exLeds[i]);
    }
}


int pauseTime = random(5,10);
bool idle = false;

int maxNumberOfLoops = NUM__LEDS + WAVE_SIZE;
int loopCounter = 0;
bool waveInProgress = true;

bool pulseInProgress = false;

int sinValue = 0;


void dimTo(int value) {
    for(int i = 0; i < NUM__LEDS; ++i)
    {
        if(hsv_leds[i].v >= value) {
            hsv_leds[i].v -= 5;
        }
    }
}

void waveEffect() {
    EVERY_N_MILLISECONDS(200) {
        for(int i = 0; i < WAVE_SIZE; ++i) {
            hsv_leds[loopCounter + i].v = 220;
        }

        if(loopCounter > 0) {
            hsv_leds[loopCounter-1].v = 100;
        }

        ++loopCounter;
        if(loopCounter == maxNumberOfLoops) {
            loopCounter = 0;
            waveInProgress = false;
            pulseInProgress = true;
        }
    }
}

void pulseEffect() {
    EVERY_N_MILLISECONDS(10)
    {
        for(int i = 0; i < NUM__EX_LEDS; ++i)
        {
            hsv_exLeds[i] = CHSV(160, 255, sin(sinValue*M_PI/180.f)*150 + 100);
        }

        ++sinValue;

        if(sinValue == 180) {
            sinValue = 0;
            pulseInProgress = false;
            pauseTime = random(5,10);
            idle = true;
        }
    }
}

void loop() {

    //TIMES_PER_SECOND(60) {
        if(waveInProgress) {

            waveEffect();
        }
        if(pulseInProgress) {

            pulseEffect();
        }
        if(idle) {
            FastLED.delay(pauseTime*1000);
            idle = false;
            waveInProgress = true;
        }

        copyToCRGB();
        FastLED.show();
//    }

}
