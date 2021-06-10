#include <FastLED.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define NUM__LEDS 13
#define NUM__EX_LEDS 1
#define NUM__COLORS 8

#define WAVE_SIZE 3

#define TIMES_PER_SECOND(x) EVERY_N_MILLISECONDS(1000/x)
#define ARRAYLENGTH(x) (sizeof(x)/sizeof(x[0]))

CRGB leds[NUM__LEDS];
CHSV hsv_leds[NUM__LEDS + WAVE_SIZE*2];

CRGB ex_leds[NUM__EX_LEDS];
CHSV hsv_exLeds[NUM__EX_LEDS];

CHSV colors[NUM__COLORS];

int currentColor = 3;
int g_brightness = 144;
int minBrightness = 16;
int maxBrightness = 240;
int step = 32;

bool running = false;

const char *ssid = "dva";
const char *password = "1234";

WiFiServer server(80);

void createColors() {
    colors[0] = CHSV(0, 255, 225);
    colors[1] = CHSV(32, 255, 225);
    colors[2] = CHSV(64, 255, 225);
    colors[3] = CHSV(96, 255, 225);
    colors[4] = CHSV(128, 255, 225);
    colors[5] = CHSV(160, 255, 225);
    colors[6] = CHSV(192, 255, 225);
    colors[7] = CHSV(224, 255, 225);
}

void reset(CHSV color){
    for(int i = 0; i < NUM__LEDS; ++i)
    {
        hsv_leds[i] = color;
    }

    for(int i = 0; i < NUM__EX_LEDS; ++i)
    {
        hsv_exLeds[i] = color;
    }
}

void setup() {
    FastLED.addLeds<NEOPIXEL, 27>(leds, NUM__LEDS);
    FastLED.addLeds<NEOPIXEL, 25>(ex_leds, NUM__EX_LEDS);

    FastLED.setBrightness(g_brightness);
    FastLED.clear();

    createColors();
    reset(colors[currentColor]);

    WiFi.softAP(ssid);
    IPAddress myIP = WiFi.softAPIP();
    running = true;
    server.begin();
}

CHSV nextColor() {
    currentColor = ++currentColor % NUM__COLORS;
    return colors[currentColor];
}

CHSV prevColor() {
    currentColor = --currentColor % NUM__COLORS;
    return colors[currentColor];
}

void shutDown() {
    server.end();
    WiFi.mode(WIFI_OFF);
    running = false;
}

void handleClient(WiFiClient client) {
    if (client) {                                   // if you get a client,
        Serial.println("New Client.");              // print a message out the serial port
        String currentLine = "";                    // make a String to hold incoming data from the client
        while (client.connected()) {                // loop while the client's connected
            if (client.available()) {               // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                if (c == '\n') {                    // if the byte is a newline character

                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        client.print("<div style=\"position: relative; max-width: 900px; margin-left: auto; margin-right: auto; background-color: #f7f7f7; box-sizing: border-box; \">");
                            client.print("<div style=\"display: flex; flex-direction: column; align-items: center; font-size: 6.5 rem;\">");

                                client.print("<div style=\"width: 100%; display: flex; flex-direction: column; align-items: center;\">");
                                    client.print("<p>Color <a href=\"/CH\">_UP_</a> </p>");
                                    client.print("<p>Color <a href=\"/CL\">DOWN</a> </p>");
                                client.print("</div>");

                                client.print("<div style=\"width: 100%; display: flex; flex-direction: column; align-items: center;\">");
                                    client.print("<p>Brightness <a href=\"/BH\">_UP_</a> </p>");
                                    client.print("<p>Brightness <a href=\"/BL\">DOWN</a> </p>");
                                client.print("</div>");

                            client.print("</div>");
                        client.print("</div>");
                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    } else {    // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                } else if (c != '\r') {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }

                if (currentLine.endsWith("GET /CH")) {
                    CHSV color = nextColor();
                    reset(color);
                }

                if (currentLine.endsWith("GET /CL")) {
                    CHSV color = prevColor();
                    reset(color);
                }

                if (currentLine.endsWith("GET /BH")) {
                    if(g_brightness <= maxBrightness)
                        g_brightness += step;
                }

                if (currentLine.endsWith("GET /BL")) {
                    if(g_brightness >= minBrightness)
                        g_brightness -= step;
                }
            }
        }
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
            hsv_leds[loopCounter + i].v = 225;
        }

        if(loopCounter > 0) {
            hsv_leds[loopCounter-1].v = 100;
        }

        if(loopCounter == maxNumberOfLoops) {
            loopCounter = 0;
            waveInProgress = false;
            pulseInProgress = true;
        }
        ++loopCounter;
    }
}

void pulseEffect() {
    EVERY_N_MILLISECONDS(10)
    {
        for(int i = 0; i < NUM__EX_LEDS; ++i)
        {
            hsv_exLeds[i].v = sin(sinValue*M_PI/180.f) * 150 + 100;
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

void loop()
{
    if(running && millis() > 3 * 60 * 1000) {
        shutDown();
    }

    WiFiClient client = server.available();   // listen for incoming clients
    handleClient(client);
    client.stop();

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
    FastLED.setBrightness(g_brightness);
    FastLED.show();
}
