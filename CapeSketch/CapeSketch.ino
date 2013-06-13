/*
 * Cape of Life
 * Adapted from http://pastebin.com/f22bfe94d and Adafruit Neopixel tutorial.
*/
 
#include <Adafruit_NeoPixel.h>  

// Columns were alternated between running up the cape and down the cape for wiring purposes. With this on, every other row will have the output flipped.
#define ALTERNATE_COLUMNS 1
#define DELAY 0
#define COLORCYCLEAMOUNT 16
#define SIZEX 7
#define SIZEY 10
byte world[SIZEX][SIZEY][3];
int oldR, oldG, oldB;
boolean firstCycle = true;
long density = 22;
 
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 6, NEO_GRB + NEO_KHZ800);
 
void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  randomSeed(analogRead(0));
  for (int i = 0; i < SIZEX; i++) {
    for (int j = 0; j < SIZEY; j++) {
      if (random(100) < density) {
        world[i][j][0] = 1;
      }
      else {
        world[i][j][0] = 0;
      }
      //world[i][j][0] = 0;
      world[i][j][1] = 0;
    }
  }
  oldR = random(255);
  oldG = random(255);
  oldB = random(255);
  //world[1][5][0] = 1;
  //world[2][6][0] = 1;
  //world[3][4][0] = 1;
  //world[3][5][0] = 1;
  //world[3][6][0] = 1;
}
 
void loop() {
  // Some example procedures showing how to display to the pixels:
  //colorWipe(strip.Color(255, 0, 0), 50); // Red
  //colorWipe(strip.Color(0, 255, 0), 50); // Green
  uint16_t i, j, k;
  int r, g, b;
  r = oldR;
  g = oldG;
  b = oldB;
  
 switch(random(3)) {
    case 0:
    r = oldR + random(COLORCYCLEAMOUNT);
    break;
    
    case 1:
    g = oldG + random(COLORCYCLEAMOUNT);
    break;
    
    case 2:
    b = oldB + random(COLORCYCLEAMOUNT);
    break;
  } 
  //r = random(255);
  //g = random(255);
  //b = random(255);
  
  
  // Fade out old generation
  for(i = 0; i<COLORCYCLEAMOUNT; i++) {
    for (j = 0; j < SIZEX; j++) {
      for (k = 0; k < SIZEY; k++) {
        if (world[j][k][2] && world[j][k][0]) {
          //setGridPixelColor(j, k, strip.Color((i*oldR)/COLORCYCLEAMOUNT, (i*g)/COLORCYCLEAMOUNT, (i*b)/COLORCYCLEAMOUNT));
        } else if (world[j][k][2]) {
          setGridPixelColor(j, k, strip.Color(oldR - (oldR * i) / COLORCYCLEAMOUNT, oldG - (oldG * i) / COLORCYCLEAMOUNT, oldB - (oldB * i) / COLORCYCLEAMOUNT));
        } else {
          setGridPixelColor(j, k, strip.Color(0, 0, 0));
        }
      }
    }
  }
  // Display current generation
  for(i = 0; i<COLORCYCLEAMOUNT; i++) {
    for (j = 0; j < SIZEX; j++) {
      for (k = 0; k < SIZEY; k++) {
        if (world[j][k][0]) {
          setGridPixelColor(j, k, strip.Color((i*r)/COLORCYCLEAMOUNT, (i*g)/COLORCYCLEAMOUNT, (i*b)/COLORCYCLEAMOUNT));
        } else { 
          setGridPixelColor(j, k, strip.Color(0, 0, 0));
        }
      }
    }
  }
  delay(DELAY);
  
  oldR = r;
  oldG = g;
  oldB = b;
 
  // Birth and death cycle
  for (int x = 0; x < SIZEX; x++) {
    for (int y = 0; y < SIZEY; y++) {
      // Default is for cell to stay the same
      world[x][y][1] = world[x][y][0];
      int count = neighbours(x, y);
      if (count == 3 && world[x][y][0] == 0) {
        // A new cell is born
        world[x][y][1] = 1;
      }
      if ((count < 2 || count > 3) && world[x][y][0] == 1) {
        // Cell dies
        world[x][y][1] = 0;
      }
    }
  }
  
  // Copy next generation into place
  for (int x = 0; x < SIZEX; x++) {
    for (int y = 0; y < SIZEY; y++) {
      world[x][y][2] = world[x][y][0];
      world[x][y][0] = world[x][y][1];
    }
  }
}
 
int neighbours(int x, int y) {
 return  world[(x + 1)][y][0] + world[(x + 1)][(y + 1)][0] +
         world[(x - 1)][y][0] + world[(x - 1)][(y + 1)][0] +
         world[x][(y + 1)][0] + world[(x + 1)][(y - 1)][0] +
         world[x][(y - 1)][0] + world[(x - 1)][(y - 1)][0];
}
 
// Convert the pixel coordinates into the actual pixel and display
void setGridPixelColor(int x, int y, uint32_t c) {
  uint32_t pixel;
  if(ALTERNATE_COLUMNS && x%2 == 1)
  {
    pixel = (x*10) + (9-y);
  } else {
    pixel = (x*10) + y;
  }
  if(x < SIZEX && y < SIZEY) {
    strip.setPixelColor(pixel, c);
  }
  strip.show();
}
 
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}