#include <Arduino.h>
#include <Fastled.h>
#include <Artnet.h>


FASTLED_USING_NAMESPACE


#define DATA_PIN    D6
#define DATA_PIN2   D5
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB


#define TYPE 1 // 0 -> Decke, 1 -> PC

#ifdef TYPE == 1

#define NUM_LEDS_PER_STRIP    232
#define NUM_STRIPS  1
#define NUM_LEDS NUM_LEDS_PER_STRIP*NUM_STRIPS
#define BRIGHTNESS          155

#endif

#ifdef TYPE == 0

#define NUM_LEDS_PER_STRIP    288
#define NUM_STRIPS  1
#define NUM_LEDS NUM_LEDS_PER_STRIP*NUM_STRIPS
#define BRIGHTNESS          200

#endif


CRGB leds[NUM_LEDS];

#define FRAMES_PER_SECOND  120


const char* ssid     = "FRITZ!Box 7590 ME_EXT";
const char* password = "07000531713653714193";

const IPAddress ip(192, 168, 1, 201);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

ArtnetWiFiReceiver artnet;
uint8_t universe = 1;  // 0 - 15