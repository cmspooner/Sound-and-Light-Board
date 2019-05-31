#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define DEBUG             true

#define UP                 0
#define DOWN               1
#define OSCILLATE          2
#define CHASE              3

#define BLUE               0
#define PIXEL_BLUE        pixels.Color(0, 0, 255)
#define RED                1
#define PIXEL_RED         pixels.Color(255, 0, 0)
#define GREEN              2
#define PIXEL_GREEN       pixels.Color(0, 255, 0)
#define WHITE              3
#define PIXEL_WHITE       pixels.Color(255, 255, 255)
#define OFF                4
#define PIXEL_OFF       pixels.Color(0, 0, 0)



#define NEOPIXEL_PIN      16
#define NUM_NEOPIXELS      7

#define LED_STRIP_PIN     10

#define SOUND0             9
#define SOUND1             8
#define SOUND2             7
#define SOUND3             6
#define SOUND4             5
#define SOUND5             4
#define SOUND6             3
#define SOUND7             2

#define TARDIS_TAKEOFF    SOUND0

#define SWITCH14          14
#define SWITCH15          15
#define SWITCH18          18
#define SWITCH19          19
#define SWITCH20          20
#define SWITCH21          21

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_NEOPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
bool pixel_isFading = false;
int pixel_brightness = 0;
int pixel_minBrightness = 0;
int pixel_maxBrightness = 255;
int pixel_fadeAmount = 1;
int pixel_fadeWait = 5;
int pixel_fadeMode = OSCILLATE;
int pixel_oscillateDir = UP;
int pixel_reps = 0;
int pixel_maxReps = 0;
int pixel_accumulatedMillis = 0;
int pixel_color = BLUE;
uint32_t pixel_colors[NUM_NEOPIXELS];


bool strip_isFading = false;
int strip_brightness = 0;
int strip_minBrightness = 0;
int strip_maxBrightness = 255;
int strip_fadeAmount = 1;
int strip_fadeWait = 5;
int strip_fadeMode = OSCILLATE;
int strip_reps = 0;
int strip_maxReps = 0;
int strip_oscillateDir = UP;
int strip_accumulatedMillis = 0;

bool sw14 = true;
bool sw14_on = false;
bool sw15 = true;
bool sw15_on = false;
bool sw18 = true;
bool sw18_on = false;
bool sw19 = true;
bool sw19_on = false;
bool sw20 = true;
bool sw20_on = false;
bool sw21 = true;
bool sw21_on = false;

unsigned long prevMillis;
unsigned long curMillis;


void setup() {
  // put your setup code here, to run once:
  if (DEBUG) Serial.begin(115200);
  
  pixels.begin();
  for (int i = 0; i < NUM_NEOPIXELS; i++){
    pixel_colors[i] = PIXEL_OFF;
  }
  
  pinMode(LED_STRIP_PIN, OUTPUT);

  pinMode(SOUND0, OUTPUT);
  digitalWrite(SOUND0, HIGH);
  pinMode(SOUND1, OUTPUT);
  digitalWrite(SOUND1, HIGH);
  pinMode(SOUND2, OUTPUT);
  digitalWrite(SOUND2, HIGH);
  pinMode(SOUND3, OUTPUT);
  digitalWrite(SOUND3, HIGH);
  pinMode(SOUND4, OUTPUT);
  digitalWrite(SOUND4, HIGH);
  pinMode(SOUND5, OUTPUT);
  digitalWrite(SOUND5, HIGH);
  pinMode(SOUND6, OUTPUT);
  digitalWrite(SOUND6, HIGH);
  pinMode(SOUND7, OUTPUT);
  digitalWrite(SOUND7, HIGH);

  pinMode(SWITCH14, INPUT_PULLUP);
  pinMode(SWITCH15, INPUT_PULLUP);
  pinMode(SWITCH18, INPUT_PULLUP);
  pinMode(SWITCH19, INPUT_PULLUP);
  pinMode(SWITCH20, INPUT_PULLUP);
  pinMode(SWITCH21, INPUT_PULLUP);

  curMillis = millis();
  prevMillis = curMillis;
}

void loop() {
  //Update Time
  prevMillis = curMillis;
  curMillis = millis();
  if (DEBUG) Serial.print("Current Millis: ");
  if (DEBUG) Serial.println(curMillis);

  //Read and set switch state
  sw14 = digitalRead(SWITCH14);
  if (sw14) sw14_on = false;
  sw15 = digitalRead(SWITCH15);
  if (sw15) sw15_on = false;
  sw18 = digitalRead(SWITCH18);
  if (sw18) sw18_on = false;
  sw19 = digitalRead(SWITCH19);
  if (sw19) sw19_on = false;
  sw20 = digitalRead(SWITCH20);
  if (sw20) sw20_on = false;
  sw21 = digitalRead(SWITCH21);
  if (sw21) sw21_on = false;

  //Check Switches and do stuff
  if (!sw14 && !sw14_on){
    sw14_on = true;
    fadeStripOscillate(5, 100, 1 , 5);
    fadePixelOscillate(10, 100, BLUE, 1.1, 5);
    play(TARDIS_TAKEOFF);
  }
  if (!sw15 && !sw15_on){
    sw15_on = true;
    stopStripFade();
    setStrip(0);
    stopPixelFade();
    setPixels(0, OFF);
  }
  
  //Do Fades
  if (pixel_isFading){
    doPixelFade();
  }
  if (strip_isFading){
    doStripFade();
  }
  

  //Light lights
  analogWrite(LED_STRIP_PIN, strip_brightness);
  drawPixels();
  delay(1);
}

//--------------------------NEOPIXEL-------------------------------------

/*
 * Fade NEOPIXEL UP from minimum to maximum over fadeTime in seconds.
 */
void fadePixelUp(int minimum, int maximum, int color, float fadeTime){
  pixel_color = color;
  pixel_isFading = true;
  pixel_fadeMode = UP;
  pixel_brightness = minimum;
  pixel_minBrightness = minimum;
  pixel_maxBrightness = maximum;
  pixel_fadeWait = (fadeTime*1000)/((maximum-minimum)/pixel_fadeAmount);
  if (DEBUG) Serial.print("Fading pixel UP from ");
  if (DEBUG) Serial.print(minimum);
  if (DEBUG) Serial.print(" to ");
  if (DEBUG) Serial.print(maximum);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(pixel_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}

/*
 * Fade NEOPIXEL UP from current to maximum over fadeTime in seconds.
 */
void fadePixelUp(int maximum, int color, float fadeTime){
  pixel_color = color;
  pixel_isFading = true;
  pixel_fadeMode = UP;
  pixel_maxBrightness = maximum;
  pixel_fadeWait = (fadeTime*1000)/((maximum-pixel_brightness)/pixel_fadeAmount);
  if (DEBUG) Serial.print("Fading pixel UP from current ");
  if (DEBUG) Serial.print(pixel_brightness);
  if (DEBUG) Serial.print(" to ");
  if (DEBUG) Serial.print(maximum);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(pixel_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}


/*
 * Fade NEOPIXEL Down from minimum to maximum over fadeTime in seconds.
 */
void fadePixelDown(int maximum, int minimum, int color, float fadeTime){
  pixel_color = color;
  pixel_isFading = true;
  pixel_fadeMode = DOWN;
  pixel_brightness = maximum;
  pixel_minBrightness = minimum;
  pixel_maxBrightness = maximum;
  pixel_fadeWait = (fadeTime*1000)/((maximum-minimum)/pixel_fadeAmount);
  if (DEBUG) Serial.print("Fading pixel UP from ");
  if (DEBUG) Serial.print(maximum);
  if (DEBUG) Serial.print(" to ");
  if (DEBUG) Serial.print(minimum);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(pixel_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}

/*
 * Fade NEOPIXEL Down from current to maximum over fadeTime in seconds.
 */
void fadePixelDown(int minimum, int color, float fadeTime){
  pixel_color = color;
  pixel_isFading = true;
  pixel_fadeMode = DOWN;
  pixel_minBrightness = minimum;
  pixel_fadeWait = (fadeTime*1000)/((pixel_brightness-minimum)/pixel_fadeAmount);
  if (DEBUG) Serial.print("Fading pixel UP from current ");
  if (DEBUG) Serial.print(pixel_brightness);
  if (DEBUG) Serial.print(" to ");
  if (DEBUG) Serial.print(minimum);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(pixel_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}

/*
 * Fade LED Neopixels UP & DOWN from minimum to maximum percent over fadeTime in seconds each way.
 */
void fadePixelOscillate(int minimum, int maximum, int color, int fadeTime){
  pixel_color = color;
  pixel_reps = 0;
  pixel_maxReps = 0;
  pixel_isFading = true;
  pixel_fadeMode = OSCILLATE;
  pixel_minBrightness = minimum;
  pixel_maxBrightness = maximum;
  pixel_fadeWait = (fadeTime*1000)/((maximum-minimum)/pixel_fadeAmount);
  if (DEBUG) Serial.print("Fading pixel between ");
  if (DEBUG) Serial.print(minimum);
  if (DEBUG) Serial.print(" and ");
  if (DEBUG) Serial.print(maximum);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(pixel_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}

/*
 * Fade LED Neopixels UP & DOWN from minimum to maximum percent over fadeTime in seconds each way.
 */
void fadePixelOscillate(int minimum, int maximum, int color, int fadeTime, int reps){
  pixel_color = color;
  pixel_reps = 0;
  pixel_maxReps = reps;
  pixel_isFading = true;
  pixel_fadeMode = OSCILLATE;
  pixel_minBrightness = minimum;
  pixel_maxBrightness = maximum;
  pixel_fadeWait = (fadeTime*1000)/((maximum-minimum)/pixel_fadeAmount);
  if (DEBUG) Serial.print("Fading pixel between ");
  if (DEBUG) Serial.print(minimum);
  if (DEBUG) Serial.print(" and ");
  if (DEBUG) Serial.print(maximum);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(pixel_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}

void setPixels(int value, int color){
  pixel_brightness = value;
  pixel_color = color;
  pixel_isFading = false;
  for(int i = 0; i<pixels.numPixels(); i++){
    if (pixel_color == BLUE) pixel_colors[i] = pixels.Color(0,0,255*pixel_brightness/100.0);
    if (pixel_color == RED) pixel_colors[i] = pixels.Color(255*pixel_brightness/100.0,0,0);
    if (pixel_color == GREEN) pixel_colors[i] = pixels.Color(0,255*pixel_brightness/100.0,0);
    if (pixel_color == WHITE) pixel_colors[i] = pixels.Color(255*pixel_brightness/100.0,255*pixel_brightness/100.0,255*pixel_brightness/100.0);
    if (pixel_color == OFF) pixel_colors[i] = pixels.Color(0, 0, 0);
  
  }
  if (DEBUG) Serial.print("Setting pixels to ");
  if (DEBUG) Serial.println(pixel_brightness);
}

/*
 * Turn off LED Strip fade effect
 */
void stopPixelFade(){
  pixel_isFading = false;
}

/*
 * NEOPIXEL Fading Code
 */
void doPixelFade(){
  if (curMillis - prevMillis + pixel_accumulatedMillis > pixel_fadeWait){
    if (DEBUG) Serial.println("Doing pixel fade");
    pixel_accumulatedMillis = 0;
    if (pixel_fadeMode == UP){
      if (pixel_brightness + pixel_fadeAmount > pixel_maxBrightness){
        pixel_brightness = pixel_maxBrightness;
        pixel_isFading = false;
      } else {
        pixel_brightness += pixel_fadeAmount;
      }
    } else if (pixel_fadeMode == DOWN){
      if (pixel_brightness - pixel_fadeAmount < pixel_minBrightness){
        pixel_brightness = pixel_minBrightness;
        pixel_isFading = false;
      } else {
        pixel_brightness -= pixel_fadeAmount;
      }
    } else if (pixel_fadeMode == OSCILLATE){
      if (pixel_oscillateDir == UP){
        if (pixel_brightness + pixel_fadeAmount > pixel_maxBrightness){
          pixel_brightness = pixel_maxBrightness;
          pixel_oscillateDir = DOWN;
        } else {
          pixel_brightness += pixel_fadeAmount;
        }
      } else {
        if (pixel_brightness - pixel_fadeAmount < pixel_minBrightness){
          pixel_brightness = pixel_minBrightness;
          pixel_oscillateDir = UP;
          if (pixel_maxReps != 0) {
            if (pixel_reps < pixel_maxReps){
              pixel_reps++;
            } else {
              stopPixelFade();
            }
          }
        } else {
          pixel_brightness -= pixel_fadeAmount;
        }
      }
    }

    if (pixel_fadeMode <= OSCILLATE){
      for(int i = 0; i<pixels.numPixels(); i++){
        if (pixel_color == BLUE) pixel_colors[i] = pixels.Color(0,0,255*pixel_brightness/100.0);
        if (pixel_color == RED) pixel_colors[i] = pixels.Color(255*pixel_brightness/100.0,0,0);
        if (pixel_color == GREEN) pixel_colors[i] = pixels.Color(0,255*pixel_brightness/100.0,0);
        if (pixel_color == WHITE) pixel_colors[i] = pixels.Color(255*pixel_brightness/100.0,255*pixel_brightness/100.0,255*pixel_brightness/100.0);
        
      }
    }
    
    if (DEBUG) Serial.println(pixel_brightness);
  } else {
    if (DEBUG) Serial.println("Waiting on pixel fade");
    pixel_accumulatedMillis += curMillis - prevMillis;
  }
}

void drawPixels(){
  for(int i = 0; i<pixels.numPixels(); i++){
    pixels.setPixelColor(i, pixel_colors[i]);
    pixels.show();
  } 
}

//--------------------------STRIP-------------------------------------

/*
 * Fade LED Strip UP from minimum to maximum over fadeTime in seconds.
 */
void fadeStripUp(int minimum, int maximum, float fadeTime){
  strip_isFading = true;
  strip_fadeMode = UP;
  strip_brightness = minimum/100.0*255;
  strip_minBrightness = minimum/100.0*255;
  strip_maxBrightness = maximum/100.0*255;
  strip_fadeWait = (fadeTime*1000)/((maximum-minimum)/strip_fadeAmount);
  if (DEBUG) Serial.print("Fading strip UP from ");
  if (DEBUG) Serial.print(minimum/100.0*255);
  if (DEBUG) Serial.print(" to ");
  if (DEBUG) Serial.print(maximum/100.0*255);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(strip_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}

/*
 * Fade LED Strip UP from current to maximum over fadeTime in seconds.
 */
void fadeStripUp(int maximum, int fadeTime){
  strip_isFading = true;
  strip_fadeMode = UP;
  strip_maxBrightness = maximum/100.0*255;
  strip_fadeWait = (fadeTime*1000)/((maximum-strip_brightness/255.0*100)/strip_fadeAmount);
  if (DEBUG) Serial.print("Fading strip UP from current ");
  if (DEBUG) Serial.print(strip_brightness);
  if (DEBUG) Serial.print(" to ");
  if (DEBUG) Serial.print(maximum/100.0*255);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(strip_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}

/*
 * Fade LED Strip DOWN from minimum to maximum over fadeTime in seconds.
 */
void fadeStripDown(int maximum, int minimum, int fadeTime){
  strip_isFading = true;
  strip_fadeMode = DOWN;
  strip_brightness = maximum/100.0*255;
  strip_minBrightness = minimum/100.0*255;
  strip_maxBrightness = maximum/100.0*255;
  strip_fadeWait = (fadeTime*1000)/((maximum-minimum)/strip_fadeAmount);
  if (DEBUG) Serial.print("Fading strip Down from ");
  if (DEBUG) Serial.print(maximum/100.0*255);
  if (DEBUG) Serial.print(" to ");
  if (DEBUG) Serial.print(minimum/100.0*255);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(strip_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}

/*
 * Fade LED Strip DOWN from current to maximum percent over fadeTime in seconds.
 */
void fadeStripDown(int minimum, int fadeTime){
  strip_isFading = true;
  strip_fadeMode = DOWN;
  strip_minBrightness = minimum/100.0*255;
  strip_fadeWait = (fadeTime*1000)/((strip_brightness-strip_minBrightness/255.0*100)/strip_fadeAmount);
  if (DEBUG) Serial.print("Fading strip Down from current ");
  if (DEBUG) Serial.print(strip_brightness);
  if (DEBUG) Serial.print(" to ");
  if (DEBUG) Serial.print(minimum/100.0*255);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(strip_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}

/*
 * Fade LED Strip UP & DOWN from minimum to maximum percent over fadeTime in seconds each way.
 */
void fadeStripOscillate(int minimum, int maximum, int fadeTime){
  strip_reps = 0;
  strip_maxReps = 0;
  strip_isFading = true;
  strip_fadeMode = OSCILLATE;
  strip_minBrightness = minimum/100.0*255;
  strip_maxBrightness = maximum/100.0*255;
  strip_fadeWait = (fadeTime*1000)/((maximum-minimum)/strip_fadeAmount);
  if (DEBUG) Serial.print("Fading strip between ");
  if (DEBUG) Serial.print(minimum/100.0*255);
  if (DEBUG) Serial.print(" and ");
  if (DEBUG) Serial.print(maximum/100.0*255);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(strip_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}

void fadeStripOscillate(int minimum, int maximum, int fadeTime, int reps){
  strip_reps = 0;
  strip_maxReps = reps;
  strip_isFading = true;
  strip_fadeMode = OSCILLATE;
  strip_minBrightness = minimum/100.0*255;
  strip_maxBrightness = maximum/100.0*255;
  strip_fadeWait = (fadeTime*1000)/((maximum-minimum)/strip_fadeAmount);
  if (DEBUG) Serial.print("Fading strip between ");
  if (DEBUG) Serial.print(minimum/100.0*255);
  if (DEBUG) Serial.print(" and ");
  if (DEBUG) Serial.print(maximum/100.0*255);
  if (DEBUG) Serial.print(", over ");
  if (DEBUG) Serial.print(fadeTime);
  if (DEBUG) Serial.print(" seconds; and waiting ");
  if (DEBUG) Serial.print(strip_fadeWait);
  if (DEBUG) Serial.println(" mil between changes.");
}

/*
 * Set LED Strip to brightness in percent.
 */
void setStrip(int value){
  strip_brightness = value/100.0*255;
  strip_isFading = false;
  if (DEBUG) Serial.print("Setting strip to ");
  if (DEBUG) Serial.println(strip_brightness);
}

/*
 * Turn off LED Strip fade effect
 */
void stopStripFade(){
  strip_isFading = false;
}

/*
 * LED Strip Fading Code
 */
void doStripFade(){
  if (curMillis - prevMillis + strip_accumulatedMillis > strip_fadeWait){
    if (DEBUG) Serial.println("Doing strip fade");
    strip_accumulatedMillis = 0;
    if (strip_fadeMode == UP){
      if (strip_brightness + strip_fadeAmount > strip_maxBrightness){
        strip_brightness = strip_maxBrightness;
        strip_isFading = false;
      } else {
        strip_brightness += strip_fadeAmount;
      }
    } else if (strip_fadeMode == DOWN){
      if (strip_brightness - strip_fadeAmount < strip_minBrightness){
        strip_brightness = strip_minBrightness;
        strip_isFading = false;
      } else {
        strip_brightness -= strip_fadeAmount;
      }
    } else if (strip_fadeMode == OSCILLATE){
      if (strip_oscillateDir == UP){
        if (strip_brightness + strip_fadeAmount > strip_maxBrightness){
          strip_brightness = strip_maxBrightness;
          strip_oscillateDir = DOWN;
        } else {
          strip_brightness += strip_fadeAmount;
        }
      } else {
        if (strip_brightness - strip_fadeAmount < strip_minBrightness){
          strip_brightness = strip_minBrightness;
          strip_oscillateDir = UP;
          if (strip_maxReps != 0) {
            if (strip_reps < strip_maxReps){
              strip_reps++;
            } else {
              stopStripFade();
            }
          }
        } else {
          strip_brightness -= strip_fadeAmount;
        }
      }
    }
    if (DEBUG) Serial.println(strip_brightness);
  } else {
    if (DEBUG) Serial.println("Waiting on strip fade");
    strip_accumulatedMillis += curMillis - prevMillis;
  }
}

//--------------------------SOUNDS-------------------------------------
/*
 * Trigger a sound saved on the sound board.
 */
void play(int sound){
  if (sound >=2 and sound <= 9){
    digitalWrite(sound, LOW);
    delay(250);
    digitalWrite(sound, HIGH);
  }
}
