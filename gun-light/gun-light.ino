#include <FastLED.h>

#define NUM__LEDS 13
#define NUM__EX_LEDS 6

#define WAVE_SIZE 3

#define TIMES_PER_SECOND(x) EVERY_N_MILLISECONDS(1000/x)
#define ARRAYLENGTH(x) (sizeof(x)/sizeof(x[0]))

CRGB leds[NUM__LEDS];
CHSV hsv_leds[NUM__LEDS + WAVE_SIZE];

CRGB ex_leds[NUM__EX_LEDS];
CHSV hsv_exLeds[NUM__EX_LEDS];

CHSV blue = CHSV(160, 255, 50);

int g_brightness = 32;

void setup() {
    FastLED.addLeds<NEOPIXEL, 33>(leds, NUM__LEDS);

    FastLED.setBrightness(g_brightness);
    FastLED.clear();
    reset(blue);
}

void reset(CHSV color)
{
    for(int i = 0; i < NUM__LEDS; ++i)
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
    for(int i=0; i < NUM__LEDS; i++)
    {
        leds[i] = CRGB(hsv_leds[i]);
    }
    for(int i=0; i < NUM__EX_LEDS; i++)
    {
        ex_leds[i] = CRGB(hsv_exLeds[i]);
    }
}


int pauseTime = random(5,10);
bool idle = false;

int maxNumberOfLoops = NUM__LEDS;
int loopCounter = 0;
bool waveInProgress = true;

bool pulseInProgress = false;
int BPM = 10;
int periodTimeInMillisec = 60000/BPM;
CEveryNMillis timer(periodTimeInMillisec);

void waveEffect() {
    for(int i = 0; i < WAVE_SIZE; ++i) {
        hsv_leds[loopCounter + i].v = 200;
    }

    if(loopCounter > 0) {
        hsv_leds[loopCounter-1].v = 50;
    }

    ++loopCounter;
    if(loopCounter == maxNumberOfLoops) {
        loopCounter = 0;
        pulseInProgress = true;
        pauseTime = random(5,10);
        timer.reset();
    }
}


void pulseEffect(){
    for(int i = 0; i < NUM__EX_LEDS; ++i)
    {
        hsv_exLeds[i] = CHSV(160, 255, beatsin16(BPM, 50, 200));
    }

    if(timer) {
        pulseInProgress = false;
        waveInProgress = true;
    }
}

void loop() {

    TIMES_PER_SECOND(60) {
        if(waveInProgress) {

            waveEffect();
        }
        if(pulseInProgress) {

            pulseEffect();
        }
        if(idle) {
            delay(pauseTime*1000);
            idle = false;
            waveInProgress = true;
        }
        copyToCRGB();
        FastLED.show();
    }

}
