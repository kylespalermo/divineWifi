//HSB to RGB functions borrowed from Kasper Kamperman, https://www.kasperkamperman.com/blog/arduino/arduino-programming-hsb-to-rgb/

#include <SPI.h> 
#include <WiFiNINA.h> //wifi library

const byte dim_curve[] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};

int hue_wheel[160] = {
    315, 316, 317, 318, 319, 320, 321, 322, 323, 324,
    325, 326, 327, 328, 329, 330, 331, 332, 333, 334,
    335, 336, 337, 338, 339, 340, 341, 342, 343, 344,
    345, 346, 347, 348, 349, 350, 351, 352, 353, 354,
    355, 356, 357, 358, 359, 0,   1,   2,   3,   4, 
    5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  26,  27,  28,  29,  30,  31,  32,  33,  34,
    35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
    45,  46,  47,  48,  49,  50,  51,  52,  53,  54,
    55,  56,  57,  58,  59,  60,  61,  62,  63,  64,
    65,  66,  67,  68,  69,  70,  71,  72,  73,  74,
    75,  76,  77,  78,  79,  80,  81,  82,  83,  84,
    85,  86,  87,  88,  89,  90,  91,  92,  93,  94,
    95,  96,  97,  98,  99,  100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
};


//set output pins
const int ledPinR = 3;  // pwm pin with red led
const int ledPinG = 5; // pwm pin with green led
const int ledPinB = 9; // pwm pin with blue led


// getRGB function stores RGB values in this array. use these values for the red, blue, green led. 


int rgb_colors[3]; 

int hue;
int saturation;
int brightness;


int blueVal = hue_wheel[0];
int redVal = hue_wheel[160];


int colorVal[2] = {0, 0};


int networkStrength[2] = {0, 0}; //array that holds last network strength and current strength
int numSsid; //number of networks


void setup() { 
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinB, OUTPUT);
  Serial.begin(9600);
}


void loop()  { 

  //scan for existing networks
  listNetworks();

  if (numSsid == 0) { //if nothing, give this error display
    blinkBlue();
  } else if (networkStrength[1] > (numSsid * -20)) { //if you've hit this fictional max value, blink a red code
    blinkRed();
  } else if (networkStrength[1] > networkStrength[0]) { //if the RSSI value goes up, increment by 10
    colorVal[1] = colorVal[0] + incrementVal; 
  } else if (networkStrength[1] < networkStrength[0]) { //if the RSSI value goes down, decrement by 10
    colorVal[1] = colorVal[0] - incrementVal;
  } else if (networkStrength[1] == networkStrength[0]) { //else send it 5 values in a random direcetion
    colorVal[1] = colorVal[0] + ((incrementVal/2)*random(-1, 2)); 
  }

  if (colorVal[1] > 160) {
    colorVal[1] = 160 - incrementVal;
    colorVal[0] = 160 - (incrementVal*2);
  } else if (colorVal[1] < 0) {
    colorVal[1] = incrementVal;
    colorVal[0] = incrementVal*2;
  }

  int colorValDelta = colorVal[1] - colorVal[0]; //create a value representing difference between new and old color value
  int colorDirection;

  if (colorValDelta > 0) {
    colorDirection = 1; } else {
      colorDirection = -1;
      };
  

  hue = hue_wheel[colorVal[0]];
  
  for (int i = 0; i < abs(colorValDelta); i++) { //this should increment from old value to new value 1 at a time
    hue = hue_wheel[colorVal[0] + colorDirection*i]; //incrementally set hue value
  
    // set HSB values
    saturation = 255;
    brightness = 255;
  
    getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB
  
    analogWrite(ledPinR, rgb_colors[0]);            // red value in index 0 of rgb_colors array
    analogWrite(ledPinG, rgb_colors[1]);            // green value in index 1 of rgb_colors array
    analogWrite(ledPinB, rgb_colors[2]);            // blue value in index 2 of rgb_colors array
    Serial.print("inc/");
    Serial.print(hue);
    Serial.println();
    delay(30);
  }

  colorVal[0] = colorVal[1];

}


void listNetworks() {

  int sumRSSI = 0;

  networkStrength[0] = networkStrength[1]; //move the last network value reading into the first place in the array
  
  //scan for existing wifi networks
  numSsid = WiFi.scanNetworks();

  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    sumRSSI += (WiFi.RSSI(thisNet));
  }

  networkStrength[1] = sumRSSI; //move the new network value reading into the last place in the array
  
}


void getRGB(int hue, int sat, int val, int colors[3]) { 
  /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
     The dim_curve is used only on brightness/value and on saturation (inverted).
     This looks the most natural.      
  */

  val = dim_curve[val];
  sat = 255-dim_curve[255-sat];

  int r;
  int g;
  int b;
  int base;

  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;  
  } else  { 

    base = ((255 - sat) * val)>>8;

    switch(hue/60) {
    case 0:
        r = val;
        g = (((val-base)*hue)/60)+base;
        b = base;
    break;

    case 1:
        r = (((val-base)*(60-(hue%60)))/60)+base;
        g = val;
        b = base;
    break;

    case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%60))/60)+base;
    break;

    case 3:
        r = base;
        g = (((val-base)*(60-(hue%60)))/60)+base;
        b = val;
    break;

    case 4:
        r = (((val-base)*(hue%60))/60)+base;
        g = base;
        b = val;
    break;

    case 5:
        r = val;
        g = base;
        b = (((val-base)*(60-(hue%60)))/60)+base;
    break;
    }

    colors[0]=r;
    colors[1]=g;
    colors[2]=b; 
  }   
}

void blinkBlue() {
  
  hue = blueVal;
  saturation = 255;
  brightness = 255;

  getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB

  analogWrite(ledPinR, rgb_colors[0]);            // red value in index 0 of rgb_colors array
  analogWrite(ledPinG, rgb_colors[1]);            // green value in index 1 of rgb_colors array
  analogWrite(ledPinB, rgb_colors[2]);            // blue value in index 2 of rgb_colors array
  Serial.print("blinkBlue");
  delay(2000);
}

void blinkRed() {
  
  hue = redVal;
  saturation = 255;
  brightness = 255;

  getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB

  analogWrite(ledPinR, rgb_colors[0]);            // red value in index 0 of rgb_colors array
  analogWrite(ledPinG, rgb_colors[1]);            // green value in index 1 of rgb_colors array
  analogWrite(ledPinB, rgb_colors[2]);            // blue value in index 2 of rgb_colors array
  Serial.print("blinkRed");
  delay(2000);
}
