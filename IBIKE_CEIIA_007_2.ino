//-----------------------------------------------------------------------------
//                               ,--.           
//   ,---,    ,---,.    ,---,   ,--/  /|    ,---,. 
//,`--.' |  ,'  .'  \,`--.' |,---,': / '  ,'  .' | 
//|   :  :,---.' .' ||   :  ::   : '/ / ,---.'   | 
//:   |  '|   |  |: |:   |  '|   '   ,  |   |   .' 
//|   :  |:   :  :  /|   :  |'   |  /   :   :  |-, 
//'   '  ;:   |    ; '   '  ;|   ;  ;   :   |  ;/| 
//|   |  ||   :     \|   |  |:   '   \  |   :   .' 
//'   :  ;|   |   . |'   :  ;|   |    ' |   |  |-, 
//|   |  ''   :  '; ||   |  ''   : |.  \'   :  ;/| 
//'   :  ||   |  | ; '   :  ||   | '_\.'|   |    \ 
//;   |.' |   :   /  ;   |.' '   : |    |   :   .' 
//'---'   |   | ,'   '---'   ;   |,'    |   | ,'   
//        `----'             '---'      `----'     
//                                                 
//  
// NeoPixel-IBIKE
//
// (WS2812 RGB LED)
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

#include <Adafruit_NeoPixel.h>
#include <Button.h>        //https://github.com/JChristensen/Button

// SETUP YOUR OUTPUT PIN AND NUMBER OF PIXELS
#define PIN_LEFT 5
#define PIN_RIGHT 6
#define PIN_FRONT 10
#define PIN_TAIL 3
#define PIN_DAY 9
#define PIN_LASER 2

#define NUM_PIXELS  24
#define BUTTON_PIN_LEFT 7       //Connect a tactile button switch (or something similar)
#define BUTTON_PIN_RIGHT 8       //Connect a tactile button switch (or something similar)
#define BUTTON_PIN_UP 11       //Connect a tactile button switch (or something similar)
#define BUTTON_PIN_DOWN 12       //Connect a tactile button switch (or something similar)
#define BUTTON_PIN_LASER 4       //Connect a tactile button switch (or something similar)


#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high unless the
                           //switch is closed, this is negative logic, i.e. a high state
                           //means the button is NOT pressed. (Assuming a normally open switch.)
#define DEBOUNCE_MS 20     //A debounce time of 20 milliseconds usually works well for tactile button switches.


Adafruit_NeoPixel left = Adafruit_NeoPixel(NUM_PIXELS, PIN_LEFT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel right = Adafruit_NeoPixel(NUM_PIXELS, PIN_RIGHT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel front = Adafruit_NeoPixel(NUM_PIXELS, PIN_FRONT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel tail = Adafruit_NeoPixel(NUM_PIXELS, PIN_TAIL, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel day = Adafruit_NeoPixel(NUM_PIXELS, PIN_DAY, NEO_GRB + NEO_KHZ800);


Button leftBtn(BUTTON_PIN_LEFT, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button rightBtn(BUTTON_PIN_RIGHT, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button upBtn(BUTTON_PIN_UP, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button downBtn(BUTTON_PIN_DOWN, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button laser(BUTTON_PIN_LASER, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
boolean ledState;          //A variable that keeps the current LED status

int showType = 0;

void setup() {
  left.begin();
    right.begin();
     front.begin();
      tail.begin();
day.begin();

  clearStripleft(); // Initialize all pixels to 'off'
    clearStripright(); // Initialize all pixels to 'off'
    clearStripfront(); // Initialize all pixels to 'off'
        clearStriptail(); // Initialize all pixels to 'off'


  delay(1000);
}

void loop() {
   
  leftBtn.read();                    //Read the button
  rightBtn.read();                    //Read the button
    upBtn.read();                    //Read the button
    downBtn.read();                    //Read the button
    laser.read();                    //Read the button

daylighton();

  if (leftBtn.wasPressed()) {
  piscaRiderleft(3, 16, 4, 0xff4d00, 0x000000); // Cycles, Speed, Width, RGB Color (original orange-red)
  clearStripleft();
  delay(10);
 }
  
  if (rightBtn.wasPressed()) {
 piscaRiderright(3, 16, 4, 0xff4d00, 0x000000); // Cycles, Speed, Width, RGB Color (original orange-red)
  clearStripright();
  delay(10);
  }
  
  if (upBtn.wasPressed()) {
  showType++;
  if (showType > 3)
        showType=0;
      frontlight(showType);
 // clearStripright();
  delay(10);
  }
  
  
   if (downBtn.wasPressed()) {
  showType++;
  if (showType > 3)
        showType=0;
      taillight(showType);
 // clearStripright();
  delay(10);
  }
  
  

    if (laser.wasReleased()) {       //If the button was released, change the LED state
        ledState = !ledState;
        digitalWrite(PIN_LASER, ledState);
    }
    
    
  
}

// Cycles - one cycle is scanning through all pixels left then right (or right then left)
// Speed - how fast one cycle is (32 with 16 pixels is default KnightRider speed)
// Width - how wide the trail effect is on the fading out LEDs.  The original display used
//         light bulbs, so they have a persistance when turning off.  This creates a trail.
//         Effective range is 2 - 8, 4 is default for 16 pixels.  Play with this.
// Color - 32-bit packed RGB color value.  All pixels will be this color.
// piscaRider(cycles, speed, width, color);


void piscaRiderleft(uint16_t cycles, uint16_t speed, uint8_t width, uint32_t color, uint32_t colorblack) {
  uint32_t old_val[NUM_PIXELS]; // up to 256 lights!
  // Larson time baby!
 for(int i = 0; i < cycles; i++){
    for (int count = NUM_PIXELS-1; count>=0; count--) {
      left.setPixelColor(count, colorblack);
      old_val[count] = colorblack;
      for(int x = count; x<=NUM_PIXELS ;x++) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        left.setPixelColor(x+1, old_val[x+1]);
      }
      left.show();
     // delay(speed);
    }
    for (int count = NUM_PIXELS-1; count>=0; count--) {
      left.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x<=NUM_PIXELS ;x++) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        left.setPixelColor(x+1, old_val[x+1]);
      }
      left.show();
      delay(speed);
    }
  }
}


void piscaRiderright(uint16_t cycles, uint16_t speed, uint8_t width, uint32_t color, uint32_t colorblack) {
  uint32_t old_val[NUM_PIXELS]; // up to 256 lights!
  // Larson time baby!
 for(int i = 0; i < cycles; i++){
    for (int count = NUM_PIXELS-1; count>=0; count--) {
      right.setPixelColor(count, colorblack);
      old_val[count] = colorblack;
      for(int x = count; x<=NUM_PIXELS ;x++) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        right.setPixelColor(x+1, old_val[x+1]);
      }
      right.show();
     // delay(speed);
    }
    for (int count = NUM_PIXELS-1; count>=0; count--) {
      right.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x<=NUM_PIXELS ;x++) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        right.setPixelColor(x+1, old_val[x+1]);
      }
      right.show();
      delay(speed);
    }
  }
}




void frontlight(int i)  {

 
switch(i){
    case 0: frontlighton();    // Black/off
            break;
    case 1: frontlightoff();  // Red
            break;
    case 2: frontlighton();    // Black/off
            break;
     case 3: frontlightoff();  // Red
            break;
}
}

void taillight(int i)  {

 
switch(i){
    case 0: taillighton();    // Black/off
            break;
    case 1: taillightoff();  // Red
            break;
    case 2: taillighton();    // Black/off
            break;
     case 3: taillightoff();  // Red
            break;
}
}

void frontlighton()  {
   
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
   for( int i = 0; i<NUM_PIXELS; i++){
    front.setPixelColor(i, 0xffffff); 
    front.show();
    

  //  delay(100); // Delay for a period of time (in milliseconds).

}
}

void frontlightoff()  {
   
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
   for( int i = 0; i<NUM_PIXELS; i++){
    front.setPixelColor(i, 0x000000); 
    front.show();
    
   // delay(10); // Delay for a period of time (in milliseconds).

}
}


void taillighton()  {
   
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
   for( int i = 0; i<NUM_PIXELS; i++){
    tail.setPixelColor(i, 0xff0000); 
    tail.show();
    

  //  delay(100); // Delay for a period of time (in milliseconds).

}
}

void taillightoff()  {
   
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
   for( int i = 0; i<NUM_PIXELS; i++){
    tail.setPixelColor(i, 0x000000); 
    tail.show();
    
   // delay(10); // Delay for a period of time (in milliseconds).

}
}

void daylighton()  {
   
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
   for( int i = 0; i<NUM_PIXELS; i++){
    day.setPixelColor(i, 0xffffff); 
    day.show();
    

  //  delay(100); // Delay for a period of time (in milliseconds).

}
}

void clearStripleft() {
  for( int i = 0; i<NUM_PIXELS; i++){
    left.setPixelColor(i, 0x000000); left.show();
  }
}

void clearStripright() {
  for( int i = 0; i<NUM_PIXELS; i++){
    right.setPixelColor(i, 0x000000); right.show();
  }
}

void clearStripfront() {
  for( int i = 0; i<NUM_PIXELS; i++){
    front.setPixelColor(i, 0x000000); front.show();
  }
}

void clearStriptail() {
  for( int i = 0; i<NUM_PIXELS; i++){
    tail.setPixelColor(i, 0x000000); tail.show();
  }
}

uint32_t dimColor(uint32_t color, uint8_t width) {
   return (((color&0xFF0000)/width)&0xFF0000) + (((color&0x00FF00)/width)&0x00FF00) + (((color&0xFF0000)/width)&0xFF0000) + (((color&0x00FF00)/width)&0x00FF00) + (((color&0xFF0000)/width)&0xFF0000) + (((color&0x00FF00)/width)&0x00FF00) + (((color&0x0000FF)/width)&0x0000FF);
}


