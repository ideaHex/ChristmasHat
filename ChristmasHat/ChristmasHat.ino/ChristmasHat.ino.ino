

//////////////////////////////////////////////////////////////////////////////////////
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

typedef ColumnMajorLayout MyPanelLayout;

#define colorSaturation 255

uint16_t effectState = 0;  // general purpose variable used to store effect state
const uint8_t PanelWidth = 12;  // 8 pixel x 8 pixel matrix of leds
const uint8_t PanelHeight = 10;
//const uint16_t PanelWidth = 12;
//const uint16_t PanelHeight = 10;
const uint8_t AnimationChannels = 1; // we only need one as all the pixels are animated at once
const uint16_t PixelCount = PanelWidth * PanelHeight; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = D4;  // make sure to set this to the correct pin, ignored for Esp8266

NeoTopology<MyPanelLayout> topo(PanelWidth, PanelHeight);
NeoPixelAnimator animations(AnimationChannels); // NeoPixel animation management object
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(PixelCount, PixelPin);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);
RgbColor lightgreen(0, colorSaturation * 0.30, 0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);
HslColor hslLightGreen(lightgreen);

const uint16_t left = 0;
const uint16_t right = PanelWidth - 1;
const uint16_t top = 0;
const uint16_t bottom = PanelHeight - 1;

#define DISPLAY_QTY         10

#define DISPLAY_NONE          0
#define DISPLAY_SPARK_WHITE   1
#define DISPLAY_FADE          2
#define DISPLAY_RAND          3
#define DISPLAY_SPARK_RAND    4
#define DISPLAY_COLUMS_RIGHT  5
#define DISPLAY_COLUMS_LEFT   6
#define DISPLAY_ROWS_DOWN     7
#define DISPLAY_ROWS_UP       8
#define DISPLAY_SPARK_RED_WHITE       9
#define DISPLAY_TEXT          10




#define INTERVAL_MIN        200 // ms
#define INTERVAL_MAX        2000 //ms

int currentCol = 0;

unsigned long previousMillis = 0;
unsigned long long interval = 1000;
unsigned int currentAnimation = DISPLAY_NONE;

unsigned long frame;

//////////////////////////////////////////
// Text

const char font[][5] = {
  {0x00, 0x00, 0x00, 0x00, 0x00}, //   0x20 32
  {0x00, 0x00, 0x6f, 0x00, 0x00}, // ! 0x21 33
  {0x00, 0x07, 0x00, 0x07, 0x00}, // " 0x22 34
  {0x14, 0x7f, 0x14, 0x7f, 0x14}, // # 0x23 35
  {0x00, 0x07, 0x04, 0x1e, 0x00}, // $ 0x24 36
  {0x23, 0x13, 0x08, 0x64, 0x62}, // % 0x25 37
  {0x36, 0x49, 0x56, 0x20, 0x50}, // & 0x26 38
  {0x00, 0x00, 0x07, 0x00, 0x00}, // ' 0x27 39
  {0x00, 0x1c, 0x22, 0x41, 0x00}, // ( 0x28 40
  {0x00, 0x41, 0x22, 0x1c, 0x00}, // ) 0x29 41
  {0x14, 0x08, 0x3e, 0x08, 0x14}, // * 0x2a 42
  {0x08, 0x08, 0x3e, 0x08, 0x08}, // + 0x2b 43
  {0x00, 0x50, 0x30, 0x00, 0x00}, // , 0x2c 44
  {0x08, 0x08, 0x08, 0x08, 0x08}, // - 0x2d 45
  {0x00, 0x60, 0x60, 0x00, 0x00}, // . 0x2e 46
  {0x20, 0x10, 0x08, 0x04, 0x02}, // / 0x2f 47
  {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0 0x30 48
  {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1 0x31 49
  {0x42, 0x61, 0x51, 0x49, 0x46}, // 2 0x32 50
  {0x21, 0x41, 0x45, 0x4b, 0x31}, // 3 0x33 51
  {0x18, 0x14, 0x12, 0x7f, 0x10}, // 4 0x34 52
  {0x27, 0x45, 0x45, 0x45, 0x39}, // 5 0x35 53
  {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 6 0x36 54
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7 0x37 55
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8 0x38 56
  {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9 0x39 57
  {0x00, 0x36, 0x36, 0x00, 0x00}, // : 0x3a 58
  {0x00, 0x56, 0x36, 0x00, 0x00}, // ; 0x3b 59
  {0x08, 0x14, 0x22, 0x41, 0x00}, // < 0x3c 60
  {0x14, 0x14, 0x14, 0x14, 0x14}, // = 0x3d 61
  {0x00, 0x41, 0x22, 0x14, 0x08}, // > 0x3e 62
  {0x02, 0x01, 0x51, 0x09, 0x06}, // ? 0x3f 63
  {0x3e, 0x41, 0x5d, 0x49, 0x4e}, // @ 0x40 64
  {0x7e, 0x09, 0x09, 0x09, 0x7e}, // A 0x41 65
  {0x7f, 0x49, 0x49, 0x49, 0x36}, // B 0x42 66
  {0x3e, 0x41, 0x41, 0x41, 0x22}, // C 0x43 67
  {0x7f, 0x41, 0x41, 0x41, 0x3e}, // D 0x44 68
  {0x7f, 0x49, 0x49, 0x49, 0x41}, // E 0x45 69
  {0x7f, 0x09, 0x09, 0x09, 0x01}, // F 0x46 70
  {0x3e, 0x41, 0x49, 0x49, 0x7a}, // G 0x47 71
  {0x7f, 0x08, 0x08, 0x08, 0x7f}, // H 0x48 72
  {0x00, 0x41, 0x7f, 0x41, 0x00}, // I 0x49 73
  {0x20, 0x40, 0x41, 0x3f, 0x01}, // J 0x4a 74
  {0x7f, 0x08, 0x14, 0x22, 0x41}, // K 0x4b 75
  {0x7f, 0x40, 0x40, 0x40, 0x40}, // L 0x4c 76
  {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // M 0x4d 77
  {0x7f, 0x04, 0x08, 0x10, 0x7f}, // N 0x4e 78
  {0x3e, 0x41, 0x41, 0x41, 0x3e}, // O 0x4f 79
  {0x7f, 0x09, 0x09, 0x09, 0x06}, // P 0x50 80
  {0x3e, 0x41, 0x51, 0x21, 0x5e}, // Q 0x51 81
  {0x7f, 0x09, 0x19, 0x29, 0x46}, // R 0x52 82
  {0x46, 0x49, 0x49, 0x49, 0x31}, // S 0x53 83
  {0x01, 0x01, 0x7f, 0x01, 0x01}, // T 0x54 84
  {0x3f, 0x40, 0x40, 0x40, 0x3f}, // U 0x55 85
  {0x0f, 0x30, 0x40, 0x30, 0x0f}, // V 0x56 86
  {0x3f, 0x40, 0x30, 0x40, 0x3f}, // W 0x57 87
  {0x63, 0x14, 0x08, 0x14, 0x63}, // X 0x58 88
  {0x07, 0x08, 0x70, 0x08, 0x07}, // Y 0x59 89
  {0x61, 0x51, 0x49, 0x45, 0x43}, // Z 0x5a 90
  {0x3c, 0x4a, 0x49, 0x29, 0x1e}, // [ 0x5b 91
  {0x02, 0x04, 0x08, 0x10, 0x20}, // \ 0x5c 92
  {0x00, 0x41, 0x7f, 0x00, 0x00}, // ] 0x5d 93
  {0x04, 0x02, 0x01, 0x02, 0x04}, // ^ 0x5e 94
  {0x40, 0x40, 0x40, 0x40, 0x40}, // _ 0x5f 95
  {0x00, 0x00, 0x03, 0x04, 0x00}, // ` 0x60 96
  {0x20, 0x54, 0x54, 0x54, 0x78}, // a 0x61 97
  {0x7f, 0x48, 0x44, 0x44, 0x38}, // b 0x62 98
  {0x38, 0x44, 0x44, 0x44, 0x20}, // c 0x63 99
  {0x38, 0x44, 0x44, 0x48, 0x7f}, // d 0x64 100
  {0x38, 0x54, 0x54, 0x54, 0x18}, // e 0x65 101
  {0x08, 0x7e, 0x09, 0x01, 0x02}, // f 0x66 102
  {0x0c, 0x52, 0x52, 0x52, 0x3e}, // g 0x67 103
  {0x7f, 0x08, 0x04, 0x04, 0x78}, // h 0x68 104
  {0x00, 0x44, 0x7d, 0x40, 0x00}, // i 0x69 105
  {0x20, 0x40, 0x44, 0x3d, 0x00}, // j 0x6a 106
  {0x00, 0x7f, 0x10, 0x28, 0x44}, // k 0x6b 107
  {0x00, 0x41, 0x7f, 0x40, 0x00}, // l 0x6c 108
  {0x7c, 0x04, 0x18, 0x04, 0x78}, // m 0x6d 109
  {0x7c, 0x08, 0x04, 0x04, 0x78}, // n 0x6e 110
  {0x38, 0x44, 0x44, 0x44, 0x38}, // o 0x6f 111
  {0x7c, 0x14, 0x14, 0x14, 0x08}, // p 0x70 112
  {0x08, 0x14, 0x14, 0x18, 0x7c}, // q 0x71 113
  {0x7c, 0x08, 0x04, 0x04, 0x08}, // r 0x72 114
  {0x48, 0x54, 0x54, 0x54, 0x20}, // s 0x73 115
  {0x04, 0x3f, 0x44, 0x40, 0x20}, // t 0x74 116
  {0x3c, 0x40, 0x40, 0x20, 0x7c}, // u 0x75 117
  {0x1c, 0x20, 0x40, 0x20, 0x1c}, // v 0x76 118
  {0x3c, 0x40, 0x30, 0x40, 0x3c}, // w 0x77 119
  {0x44, 0x28, 0x10, 0x28, 0x44}, // x 0x78 120
  {0x0c, 0x50, 0x50, 0x50, 0x3c}, // y 0x79 121
  {0x44, 0x64, 0x54, 0x4c, 0x44}, // z 0x7a 122
  {0x00, 0x08, 0x36, 0x41, 0x41}, // { 0x7b 123
  {0x00, 0x00, 0x7f, 0x00, 0x00}, // | 0x7c 124
  {0x41, 0x41, 0x36, 0x08, 0x00}, // } 0x7d 125
  {0x04, 0x02, 0x04, 0x08, 0x04}, // ~ 0x7e 126
};

const char snow[][5] = {
  {0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x02, 0x00, 0x00},
  {0x04, 0x00, 0x06, 0x00, 0x04},
  {0x04, 0x08, 0x0E, 0x08, 0x04},
  {0x14, 0x08, 0x1E, 0x08, 0x14},
  {0x14, 0x08, 0x1E, 0x08, 0x14},
  {0x14, 0x08, 0x5E, 0x08, 0x14},
  {0x14, 0x08, 0x1E, 0x08, 0x14},
  {0x14, 0x08, 0x1E, 0x08, 0x14},
  {0x04, 0x08, 0x0E, 0x08, 0x04},
  {0x04, 0x00, 0x06, 0x00, 0x04},
  {0x00, 0x00, 0x02, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00},
};

void display_col(unsigned char col_number, unsigned char col_value);
void display_char (char character);

#define MESSAGE1_LENGTH 36
#define MESSAGE2_LENGTH 7
#define NUMBER_OF_COLUMS PanelWidth
#define FONT_WIDTH  5

char message1[MESSAGE1_LENGTH] = {32, 'M', 'E', 'R', 'R', 'Y', 32, 'C', 'H', 'R', 'I', 'S', 'T', 'M', 'A', 'S', 32, '&', 32, 'A', 32, 'H', 'A', 'P', 'P', 'Y', 32, 'N', 'E', 'W', 32, 'Y', 'E', 'A', 'R', 32};
char message2[MESSAGE2_LENGTH] = {32, '*', '#', '*', '#', '*', '#'};

unsigned char ptr = 0;
unsigned char font_ptr = 0;
unsigned char char_ptr = 0;
unsigned char col_ptr = 0;
unsigned char temp_ptr = 0;
//unsigned char i, j, k;
char temp[NUMBER_OF_COLUMS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char message_incomplete = 1;


////////////////////////


// what is stored for state is specific to the need, in this case, the colors.
// basically what ever you need inside the animation update function
struct MyAnimationState
{
  RgbColor StartingColor;
  RgbColor EndingColor;
};

// one entry per pixel to match the animation timing manager
MyAnimationState animationState[AnimationChannels];

void SetRandomSeed()
{
  uint32_t seed;

  // random works best with a seed that can use 31 bits
  // analogRead on a unconnected pin tends toward less than four bits
  seed = analogRead(0);
  delay(1);

  for (int shifts = 3; shifts < 31; shifts += 3)
  {
    seed ^= analogRead(0) << shifts;
    delay(1);
  }

  randomSeed(seed);
}
// simple blend function
void BlendAnimUpdate(const AnimationParam& param)
{
  // this gets called for each animation on every time step
  // progress will start at 0.0 and end at 1.0
  // we use the blend function on the RgbColor to mix
  // color based on the progress given to us in the animation
  RgbColor updatedColor = RgbColor::LinearBlend(
                            animationState[param.index].StartingColor,
                            animationState[param.index].EndingColor,
                            param.progress);

  // apply the color to the strip
  for (uint16_t pixel = 0; pixel < PixelCount; pixel++)
  {
    strip.SetPixelColor(pixel, updatedColor);
  }
}

void FadeInFadeOutRinseRepeat(float luminance)
{
  if (effectState == 0)
  {
    // Fade upto a random color
    // we use HslColor object as it allows us to easily pick a hue
    // with the same saturation and luminance so the colors picked
    // will have similiar overall brightness
    RgbColor target = HslColor(random(360) / 360.0f, 1.0f, luminance);
    uint16_t time = random(800, 2000);

    animationState[0].StartingColor = strip.GetPixelColor(0);
    animationState[0].EndingColor = target;

    animations.StartAnimation(0, time, BlendAnimUpdate);
  }
  else if (effectState == 1)
  {
    // fade to black
    uint16_t time = random(600, 700);

    animationState[0].StartingColor = strip.GetPixelColor(0);
    animationState[0].EndingColor = RgbColor(0);

    animations.StartAnimation(0, time, BlendAnimUpdate);
  }

  // toggle to the next effect state
  effectState = (effectState + 1) % 2;
}

//////////////////////////////////////////////////////////////////////////////////////////


void SingleColour (HslColor colour)
{
  for (int pixel = 0; pixel < PixelCount; pixel++)
  {
    strip.SetPixelColor(pixel, colour);
  }
  //strip.Show();
}

void SingleColum (HslColor colour1, HslColor colour2, uint16_t col)
{
  for (int pixel = 0; pixel < PixelCount; pixel++)
  {
    if ( pixel < (col * PanelHeight)) strip.SetPixelColor(pixel, colour2);
    else if ( pixel >= (col * PanelHeight) + PanelHeight) strip.SetPixelColor(pixel, colour2);
    else strip.SetPixelColor(pixel, colour1);
  }
  strip.Show();
}

void fadeInAndOut(void)
{

  if (animations.IsAnimating())
  {
    // the normal loop just needs these two to run the active animations
    animations.UpdateAnimations();
    strip.Show();
  }
  else
  {
    // no animation runnning, start some
    //
    FadeInFadeOutRinseRepeat(0.2f); // 0.0 = black, 0.25 is normal, 0.5 is bright
  }
}

void Sparkle(HslColor colour1, HslColor colour2)
{
  long randNumber = random(10, 20);

  SingleColour(colour2);
  for (int i = random(0, 5); i > 0; i--) // loop a random number of times
  {
    strip.SetPixelColor(random(PixelCount), colour1);
  }
  delay(random(10, 60));
  strip.Show();
}

void RandomSparkle(HslColor backColour)
{
  //long randNumber = random(10, 20);
  RgbColor spark;

  SingleColour(backColour);
  for (int i = random(0, 5); i > 0; i--) // loop a random number of times
  {
    spark = HslColor(random(360) / 360.0f, 1.0f, 0.2f);
    strip.SetPixelColor(random(PixelCount), spark);
  }
  delay(random(20, 50));
  strip.Show();
}

void SingleRow (HslColor colour1, HslColor colour2, uint16_t row)
{
  SingleColour(colour2);

  for (int pixel = 0; pixel < PanelWidth; pixel++)
  {
    strip.SetPixelColor(row + (pixel * PanelHeight), colour1);
  }
  strip.Show();

  //strip.SetPixelColor(topo.Map(left, top), white);
}

void RollingRows(HslColor colour1, HslColor colour2, int numRows)
{
  for (int i = 0; i < PanelHeight ; i++)
  {
    SingleColour(colour2);
    for (int j = 0; j < numRows; j++)
    {
      //SingleRow(green,blue,i+j);

      for (int pixel = 0; pixel < PanelWidth; pixel++)
      {
        strip.SetPixelColor(i + j + (pixel * PanelHeight), colour1);
      }
    }
    strip.Show();
    delay(50);
  }
}


void RollingCols(HslColor colour1, HslColor colour2, int numCols)
{
  for (int i = 0; i < PanelWidth ; i++)
  {
    SingleColum(red, green, i);
    delay(30);
  }
}

///////////////////////
// Message stuff
void display_char (char character)
{
  unsigned char col = 0;

  for (col = 0; col < 5; col++)
  {
    display_col(col + 1, font[character - 32][col]); // Add one to centre in matrix
    //delay(4);//__delay_ms(4);
  }

}
void display_col(unsigned char col_number, unsigned char col_value)
{
  for (int r = 0; r < PanelHeight; r++)
  {
    //    Serial.print("col_NUM ");
    //    Serial.print(col_number, DEC);
    //    Serial.print("col_value ");
    //    Serial.print(col_value, BIN);
    //    Serial.print(", bit ");
    //    Serial.print(1 << r, BIN);
    //    Serial.print(", result ");
    //    Serial.println(col_value & (1 << r), BIN);
    if (col_value & (1 << r - 1)) strip.SetPixelColor(topo.Map(col_number, r), red);
    else          strip.SetPixelColor(topo.Map(col_number, r), lightgreen);
    //strip.Show();
  }
}
/////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  while (!Serial); // wait for serial attach

  Serial.println();
  Serial.println("Initializing...");
  Serial.flush();

  ////////////////////////////
  // Message stuff
  while (temp_ptr < NUMBER_OF_COLUMS) // initialize temp
  {
    temp[temp_ptr++] = font[message1[char_ptr] - 32][font_ptr++];
    if (font_ptr > FONT_WIDTH)
    {
      font_ptr = 0;
      temp[++temp_ptr] = 0; // Add Space
      char_ptr++;       // Move to next character in message
    }
  }
  message_incomplete = 0;
  /////////////////////////

  // this resets all the neopixels to an off state
  strip.Begin();
  strip.Show();

  SetRandomSeed();
}


void loop()
{
  unsigned long currentMillis = millis();


//  if (message_incomplete == 0)
//  {
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      //if (currentAnimation++ >= DISPLAY_QTY)currentAnimation = 1;
      currentAnimation = random(1, DISPLAY_QTY +3);
      Serial.print("currentAnimation ");
      Serial.print(currentAnimation); 
      if(currentAnimation >= DISPLAY_QTY )currentAnimation = DISPLAY_TEXT;
      
      interval = random(INTERVAL_MIN, INTERVAL_MAX) * 10;

      frame = (unsigned long)random(20, 200);
      Serial.print(", frame ");
      Serial.println(frame);

      //        RgbColor randCol1(random(colorSaturation), random(colorSaturation), random(colorSaturation));
      //    HslColor hslrandCol1(randCol);
      //        RgbColor randCol2(random(colorSaturation), random(colorSaturation), random(colorSaturation));
      //    HslColor hslrandCol2(randCol);
    }
//  }

  switch (currentAnimation)
  {
    case DISPLAY_NONE:
      break;

    case DISPLAY_SPARK_WHITE:
      Sparkle(white, lightgreen);
      break;

   case DISPLAY_SPARK_RED_WHITE:
      Sparkle(white, red);
      break;

    case DISPLAY_FADE:
      fadeInAndOut();
      break;

    case DISPLAY_RAND:
      RandomSparkle(lightgreen);
      break;

    case DISPLAY_ROWS_DOWN:
      {
        SingleColour(green);

        for (int i = 0; i < PanelWidth ; i++)
        {
          strip.SetPixelColor(topo.Map(i, 0), red);
          strip.SetPixelColor(topo.Map(i, 3), red);
          strip.SetPixelColor(topo.Map(i, 6), red);
          strip.SetPixelColor(topo.Map(i, 9), red);

        }
        strip.Show();
        delay(frame);
        SingleColour(green);
        for (int i = 0; i < PanelWidth ; i++)
        {
          strip.SetPixelColor(topo.Map(i, 1), red);
          strip.SetPixelColor(topo.Map(i, 4), red);
          strip.SetPixelColor(topo.Map(i, 7), red);
          //strip.SetPixelColor(topo.Map(i, 10), red);
        }
        strip.Show();
        delay(frame);
        SingleColour(green);
        for (int i = 0; i < PanelWidth ; i++)
        {
          strip.SetPixelColor(topo.Map(i, 2), red);
          strip.SetPixelColor(topo.Map(i, 5), red);
          strip.SetPixelColor(topo.Map(i, 8), red);
          //strip.SetPixelColor(topo.Map(i, 11), red);
        }
        strip.Show();
        delay(frame);
      }
      break;


    case DISPLAY_ROWS_UP:
      {
        SingleColour(red);
        for (int i = PanelWidth; i >= 0 ; i--)
        {
          strip.SetPixelColor(topo.Map(i, 0), green);
          strip.SetPixelColor(topo.Map(i, 3), green);
          strip.SetPixelColor(topo.Map(i, 6), green);
          strip.SetPixelColor(topo.Map(i, 9), green);

        }
        strip.Show();
        delay(frame);

        SingleColour(red);
        for (int i = PanelWidth; i >= 0 ; i--)
        {
          strip.SetPixelColor(topo.Map(i, 2), green);
          strip.SetPixelColor(topo.Map(i, 5), green);
          strip.SetPixelColor(topo.Map(i, 8), green);
          //strip.SetPixelColor(topo.Map(i, 11), red);
        }
        strip.Show();
        delay(frame);
        SingleColour(red);
        for (int i = PanelWidth; i >= 0 ; i--)
        {
          strip.SetPixelColor(topo.Map(i, 1), green);
          strip.SetPixelColor(topo.Map(i, 4), green);
          strip.SetPixelColor(topo.Map(i, 7), green);
          //strip.SetPixelColor(topo.Map(i, 10), red);
        }
        strip.Show();
        delay(frame);
      }
      break;



    case DISPLAY_COLUMS_RIGHT:
      {
        SingleColour(green);
        for (int i = 0; i < PanelHeight ; i++)
        {
          strip.SetPixelColor(topo.Map(0, i), red);
          strip.SetPixelColor(topo.Map(3, i), red);
          strip.SetPixelColor(topo.Map(6, i), red);
          strip.SetPixelColor(topo.Map(9, i), red);
        }
        strip.Show();
        delay(frame);
        SingleColour(green);
        for (int i = 0; i < PanelHeight ; i++)
        {
          strip.SetPixelColor(topo.Map(1, i), red);
          strip.SetPixelColor(topo.Map(4, i), red);
          strip.SetPixelColor(topo.Map(7, i), red);
          strip.SetPixelColor(topo.Map(10, i), red);
        }
        strip.Show();
        delay(frame);
        SingleColour(green);
        for (int i = 0; i < PanelHeight ; i++)
        {
          strip.SetPixelColor(topo.Map(2, i), red);
          strip.SetPixelColor(topo.Map(5, i), red);
          strip.SetPixelColor(topo.Map(8, i), red);
          strip.SetPixelColor(topo.Map(11, i), red);
        }
        strip.Show();
        delay(frame);
      }
      break;

    case DISPLAY_COLUMS_LEFT:
      {
        SingleColour(green);
        for (int i = 0; i < PanelHeight ; i++)
        {
          strip.SetPixelColor(topo.Map(0, i), red);
          strip.SetPixelColor(topo.Map(3, i), red);
          strip.SetPixelColor(topo.Map(6, i), red);
          strip.SetPixelColor(topo.Map(9, i), red);
        }
        strip.Show();
        delay(frame);

        SingleColour(green);
        for (int i = 0; i < PanelHeight ; i++)
        {
          strip.SetPixelColor(topo.Map(2, i), red);
          strip.SetPixelColor(topo.Map(5, i), red);
          strip.SetPixelColor(topo.Map(8, i), red);
          strip.SetPixelColor(topo.Map(11, i), red);
        }
        strip.Show();
        delay(frame);
        SingleColour(green);
        for (int i = 0; i < PanelHeight ; i++)
        {
          strip.SetPixelColor(topo.Map(1, i), red);
          strip.SetPixelColor(topo.Map(4, i), red);
          strip.SetPixelColor(topo.Map(7, i), red);
          strip.SetPixelColor(topo.Map(10, i), red);
        }
        strip.Show();
        delay(frame);
      }
      break;


    case DISPLAY_SPARK_RAND:
      {
        RgbColor randCol(random(colorSaturation * 0.4), random(colorSaturation * 0.4), random(colorSaturation * 0.4));
        HslColor hslrandCol(randCol);
        SingleColour (hslrandCol);
        strip.Show();
        delay(random(100));
      }
      break;

    case DISPLAY_TEXT:

      while (message_incomplete)
      {
        // Scroll message
        // display temp to matrix
        for (int i = 0; i < NUMBER_OF_COLUMS; i++)
        {
          display_col(i, temp[i]);

          //Serial.print("/r/n,   columnumber ");
          //Serial.println(i);
          delay(8);
        }
        strip.Show();

        // update cols 0 to 5
        for (temp_ptr = 0; temp_ptr < NUMBER_OF_COLUMS - 1; temp_ptr++) // move 0 > 1...4 > 5
        {
          temp[temp_ptr] = temp[temp_ptr + 1];  //
        }

        if (font_ptr >= FONT_WIDTH)
        {
          font_ptr = 0;
          temp[temp_ptr] = 0; // Add Space
          char_ptr++;       // Move to next character in message
          if (char_ptr >= MESSAGE1_LENGTH)
          {
            char_ptr = 0;
            message_incomplete = 0;
          }
        }
        else
        {
          temp[temp_ptr] = font[message1[char_ptr] - 32][font_ptr];
          font_ptr++;
        }
      }
      //SingleColour(green);//PORTA = 0;          // Blank display .....
      message_incomplete = 1;

      break;


  }
}

