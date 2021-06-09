#include <FastLED.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define NUM__OF_LEDS 4
#define NUM__COLORS 3

// Set these to your desired credentials.
const char *ssid = "dva";
const char *password = "1234";

WiFiServer server(80);

IPAddress local_IP(192, 168, 1, 184);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

CRGB leds[NUM__OF_LEDS];
CHSV hsv_leds[NUM__OF_LEDS];

CHSV colors[NUM__COLORS];

int currentColor = 0;
int g_brightness = 32;

void copyToCRGB()
{
    for(int i=0; i < NUM__OF_LEDS; i++)
    {
        leds[i] = CRGB(hsv_leds[i]);
    }
}

void reset(int currentColorIndex)
{
    colors[0] = CHSV(30, 255, 200);
    colors[1] = CHSV(90, 255, 200);
    colors[2] = CHSV(160, 255, 200);

    for(int i = 0; i < NUM__OF_LEDS; ++i)
    {
        hsv_leds[i] = colors[currentColorIndex];
    }
}

CHSV nextColor(int currentColorIndex) {
    int nextColorIndex = ++currentColorIndex % 3;
    return colors[nextColorIndex];
}

CHSV prevColor(int currentColorIndex) {
    int prevColorIndex = --currentColorIndex % 3;
    return colors[prevColorIndex];
}

void setup() {
    FastLED.addLeds<NEOPIXEL, 33>(leds, NUM__OF_LEDS);

    FastLED.setBrightness(g_brightness);
    FastLED.clear();

    reset(currentColor);

    Serial.begin(115200);
    Serial.println();
    Serial.println("Configuring access point...");

    if (!WiFi.config(local_IP, gateway, subnet)) {
        Serial.println("STA Failed to configure");
    }

    WiFi.softAP(ssid);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.begin();

    Serial.println("Server started");
}

void loop() {
    WiFiClient client = server.available();   // listen for incoming clients

    if (client) {                             // if you get a client,
        Serial.println("New Client.");           // print a message out the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
        while (client.connected()) {            // loop while the client's connected
            if (client.available()) {             // if there's bytes to read from the client,
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

                        // the content of the HTTP response follows the header:
                        client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
                        client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

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

                // Check to see if the client request was "GET /H" or "GET /L":
                if (currentLine.endsWith("GET /H")) {
                    hsv_leds[0] = nextColor(currentColor);
                    currentColor++;
                }
                if (currentLine.endsWith("GET /L")) {
                    hsv_leds[0] = prevColor(currentColor);
                    currentColor--;
                }
            }
        }

        client.stop();

        copyToCRGB();
        FastLED.show();
    }
}
