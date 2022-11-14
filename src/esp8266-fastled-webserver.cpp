#include <Arduino.h>
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <esp8266-fastled-webserver.h>




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

WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;



void setup() {
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  delay(3000); // 3 second delay for recovery  

  //FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, 0, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN2,COLOR_ORDER>(leds, NUM_LEDS, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN2,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.

typedef void (*SimplePatternList[])();

SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, cylon, Fire2012 };
//SimplePatternList gPatterns = {  };
//SimplePatternList gPatterns = { white };
//SimplePatternList gPatterns = { black };
//SimplePatternList gPatterns = { discostrobe };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

boolean autoSwitch = true;

void loop(){
   gPatterns[gCurrentPatternNumber]();   // Call the current pattern function once, updating the 'leds' array
  
  FastLED.show();  // send the 'leds' array out to the actual LED strip  
  FastLED.delay(1000/FRAMES_PER_SECOND); // insert a delay to keep the framerate modest

  // do some periodic updates
  EVERY_N_MILLISECONDS( 16.67 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  
  if (autoSwitch) {
    EVERY_N_SECONDS( 13 ) { nextPattern(); } // change patterns periodically
  }
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
 
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 FastLED GUI</h1>");
            
            //client.println("<p> autoSwitch" + (autoSwitch ? "active" : "passive") + "</p>");
            client.println("<p>autoSwitch</p>");
            if (true) {
                  client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
                    client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
          
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

#define COOLING  80  //Fire2012
#define SPARKING 200 


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


void nextPattern(){  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns); 
}



void rainbow() {  // FastLED's built-in rainbow generator  
  fill_rainbow( leds, NUM_LEDS, gHue, 3);
}

void white(){
  fill_solid(leds,NUM_LEDS, CRGB(255,255,255));
  }

void black(){
  fill_solid(leds,NUM_LEDS, CRGB(0,0,0));
  }

  

void cylon(){
  static uint8_t hue = 0;
  
  //Serial.print("x");
    
  for(int i = 0; i < NUM_LEDS; i++) {  // First slide the led in one direction
    
    leds[i] = CHSV(hue++, 255, 255);  // Set the i'th led to red    
    FastLED.show();    // Show the leds
    
    // leds[i] = CRGB::Black; // now that we've shown the leds, reset the i'th led to black
    
    delay(2); // Wait a little bit before we loop around and do it again
  }
  //Serial.print("x");

  
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {  // Now go in the other direction.  
    
    leds[i] = CHSV(hue++, 255, 255);  // Set the i'th led to red     
    FastLED.show(); // Show the leds
    
    // leds[i] = CRGB::Black; // now that we've shown the leds, reset the i'th led to black
    
    delay(2); // Wait a little bit before we loop around and do it again
  }
}

void rainbowWithGlitter() // built-in FastLED rainbow, plus some random sparkly glitter
{  
  rainbow();
  addGlitter(160);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() // random colored speckles that blink in and fade smoothly
{  
  fadeToBlackBy( leds, NUM_LEDS, 2);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()  // a colored dot sweeping back and forth, with fading trails
{  
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 30, 0, NUM_LEDS-16 );
  leds[pos] += CHSV( gHue, 255, 192);
  int pos2 = beatsin16( 20, 0, NUM_LEDS-56 )+40;
  leds[pos2] += CHSV( gHue, 0, 63);
}

void bpm()  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
{  
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*10), beat-gHue+(i*4));
  }
}

void juggle() { // eight colored dots, weaving in and out of sync with each other  
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void Fire2012() // Array of temperature readings at each simulation cell
{
  static uint8_t heat[NUM_LEDS];
  bool gReverseDirection = false;
  
    for( int i = 0; i < NUM_LEDS; i++) {  // Step 1.  Cool down every cell a little
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }  
    
    for( int k= NUM_LEDS - 1; k >= 2; k--) {  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
        
    if( random8() < SPARKING ) {  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }
    
    for( int j = 0; j < NUM_LEDS; j++) {  // Step 4.  Map from heat cells to LED colors
      CRGB color = HeatColor( heat[j]);      
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}


#define ZOOMING_BEATS_PER_MINUTE 128

void discostrobe()
{
  // First, we black out all the LEDs
  fill_solid( leds, NUM_LEDS, CRGB::Black);

  // To achive the strobe effect, we actually only draw lit pixels
  // every Nth frame (e.g. every 4th frame).  
  // sStrobePhase is a counter that runs from zero to kStrobeCycleLength-1,
  // and then resets to zero.  
  const uint8_t kStrobeCycleLength = 2; // light every Nth frame
  static uint8_t sStrobePhase = 0;
  sStrobePhase = sStrobePhase + 1;
  if( sStrobePhase >= kStrobeCycleLength ) { 
    sStrobePhase = 0; 
  }

  // We only draw lit pixels when we're in strobe phase zero; 
  // in all the other phases we leave the LEDs all black.
  if( sStrobePhase == 0 ) {

    // The dash spacing cycles from 4 to 9 and back, 8x/min (about every 7.5 sec)
    uint8_t dashperiod= beatsin8( 8/*cycles per minute*/, 4,10);
    // The width of the dashes is a fraction of the dashperiod, with a minimum of one pixel
    uint8_t dashwidth = (dashperiod / 4) + 1;
    
    // The distance that the dashes move each cycles varies 
    // between 1 pixel/cycle and half-the-dashperiod/cycle.
    // At the maximum speed, it's impossible to visually distinguish
    // whether the dashes are moving left or right, and the code takes
    // advantage of that moment to reverse the direction of the dashes.
    // So it looks like they're speeding up faster and faster to the
    // right, and then they start slowing down, but as they do it becomes
    // visible that they're no longer moving right; they've been 
    // moving left.  Easier to see than t o explain.
    //
    // The dashes zoom back and forth at a speed that 'goes well' with
    // most dance music, a little faster than 120 Beats Per Minute.  You
    // can adjust this for faster or slower 'zooming' back and forth.
    uint8_t zoomBPM = ZOOMING_BEATS_PER_MINUTE;
    int8_t  dashmotionspeed = beatsin8( (zoomBPM /2), 1,dashperiod);
    // This is where we reverse the direction under cover of high speed
    // visual aliasing.
    if( dashmotionspeed >= (dashperiod/2)) { 
      dashmotionspeed = 0 - (dashperiod - dashmotionspeed );
    }

    
    // The hueShift controls how much the hue of each dash varies from 
    // the adjacent dash.  If hueShift is zero, all the dashes are the 
    // same color. If hueShift is 128, alterating dashes will be two
    // different colors.  And if hueShift is range of 10..40, the
    // dashes will make rainbows.
    // Initially, I just had hueShift cycle from 0..130 using beatsin8.
    // It looked great with very low values, and with high values, but
    // a bit 'busy' in the middle, which I didnt like.
    //   uint8_t hueShift = beatsin8(2,0,130);
    //
    // So instead I layered in a bunch of 'cubic easings'
    // (see http://easings.net/#easeInOutCubic )
    // so that the resultant wave cycle spends a great deal of time
    // "at the bottom" (solid color dashes), and at the top ("two
    // color stripes"), and makes quick transitions between them.
    uint8_t cycle = beat8(2); // two cycles per minute
    uint8_t easedcycle = ease8InOutCubic( ease8InOutCubic( cycle));
    uint8_t wavecycle = cubicwave8( easedcycle);
    uint8_t hueShift = scale8( wavecycle,130);


    // Each frame of the animation can be repeated multiple times.
    // This slows down the apparent motion, and gives a more static
    // strobe effect.  After experimentation, I set the default to 1.
    uint8_t strobesPerPosition = 1; // try 1..4


    // Now that all the parameters for this frame are calculated,
    // we call the 'worker' function that does the next part of the work.
    discoWorker( dashperiod, dashwidth, dashmotionspeed, strobesPerPosition, hueShift);
  }  
}


// discoWorker updates the positions of the dashes, and calls the draw function
//
void discoWorker( 
    uint8_t dashperiod, uint8_t dashwidth, int8_t  dashmotionspeed,
    uint8_t stroberepeats,
    uint8_t huedelta)
 {
  static uint8_t sRepeatCounter = 0;
  static int8_t sStartPosition = 0;
  static uint8_t sStartHue = 0;

  // Always keep the hue shifting a little
  sStartHue += 1;

  // Increment the strobe repeat counter, and
  // move the dash starting position when needed.
  sRepeatCounter = sRepeatCounter + 1;
  if( sRepeatCounter>= stroberepeats) {
    sRepeatCounter = 0;
    
    sStartPosition = sStartPosition + dashmotionspeed;
    
    // These adjustments take care of making sure that the
    // starting hue is adjusted to keep the apparent color of 
    // each dash the same, even when the state position wraps around.
    if( sStartPosition >= dashperiod ) {
      while( sStartPosition >= dashperiod) { sStartPosition -= dashperiod; }
      sStartHue  -= huedelta;
    } else if( sStartPosition < 0) {
      while( sStartPosition < 0) { sStartPosition += dashperiod; }
      sStartHue  += huedelta;
    }
  }

  // draw dashes with full brightness (value), and somewhat
  // desaturated (whitened) so that the LEDs actually throw more light.
  const uint8_t kSaturation = 208;
  const uint8_t kValue = 255;

  // call the function that actually just draws the dashes now
  drawRainbowDashes( sStartPosition, NUM_LEDS-1, 
                     dashperiod, dashwidth, 
                     sStartHue, huedelta, 
                     kSaturation, kValue);
}


// drawRainbowDashes - draw rainbow-colored 'dashes' of light along the led strip:
//   starting from 'startpos', up to and including 'lastpos'
//   with a given 'period' and 'width'
//   starting from a given hue, which changes for each successive dash by a 'huedelta'
//   at a given saturation and value.
//
//   period = 5, width = 2 would be  _ _ _ X X _ _ _ Y Y _ _ _ Z Z _ _ _ A A _ _ _ 
//                                   \-------/       \-/
//                                   period 5      width 2
//
static void drawRainbowDashes( 
  uint8_t startpos, uint16_t lastpos, uint8_t period, uint8_t width, 
  uint8_t huestart, uint8_t huedelta, uint8_t saturation, uint8_t value)
{
  uint8_t hue = huestart;
  for( uint16_t i = startpos; i <= lastpos; i += period) {
    CRGB color = CHSV( hue, saturation, value);
    
    // draw one dash
    uint16_t pos = i;
    for( uint8_t w = 0; w < width; w++) {
      leds[ pos ] = color;
      pos++;
      if( pos >= NUM_LEDS) {
        break;
      }
    }
    
    hue += huedelta;
  }
}


// delayToSyncFrameRate - delay how many milliseconds are needed
//   to maintain a stable frame rate.
static void delayToSyncFrameRate( uint8_t framesPerSecond)
{
  static uint32_t msprev = 0;
  uint32_t mscur = millis();
  uint16_t msdelta = mscur - msprev;
  uint16_t mstargetdelta = 1000 / framesPerSecond;
  if( msdelta < mstargetdelta) {
    delay( mstargetdelta - msdelta);
  }
  msprev = mscur;
}
