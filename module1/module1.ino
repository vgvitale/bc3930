//this code will show you the battery charge left on the screen
//and dim the screen (to conserve power)
//and go to sleep when you press the left builtin button
//to wake it up again, just hit the reset button on the side


//download as zip and add to libraries folder in Arduino folder https://github.com/pangodream/18650CL
#include <Pangodream_18650_CL.h> 

#define ADC_PIN 34
#define CONV_FACTOR 1.8
#define READS 20
Pangodream_18650_CL BL(ADC_PIN, CONV_FACTOR, READS);

#include <TFT_eSPI.h>
#include <SPI.h>

#define BUTTON_PIN_BITMASK 0x800000000 // 2^35 in hex

void IRAM_ATTR goToSleep() {
  esp_deep_sleep_start();
}


// Example to show how to use the TFT_eSPI library to create banners of horizontally scrolling text.
// Steve M. Potter stevempotter.tech 11 May 2022
// Many thanks to Bodmer! https://github.com/Bodmer/TFT_eSPI
// The Sprite stuff can be found in the Extensions folder. It uses all the same functions.
// Don't forget to edit User_Setup.h in the TFT_eSPI library folder to suit your display.
// If you just get blackness, that probably means either you got the parameters wrong for your board, or
// perhaps you have another copy of TFT_eSPI on your computer that the Arduino IDE was using during compilation.
// Have a careful look at the compile log to see which library it used and edit User_Setup.h in THAT one.

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI(); // create an instance of class TFT_eSPI called tft. You may call it whatever you wish.
TFT_eSprite ScrollTextSprite = TFT_eSprite(&tft); // Create a Sprite object called ScrollTextSprite
// A Sprite is a 'virtual screen' on which to write words (or shapes or images or whatever) using the same functions you would use for the real screen.
// It gets created in memory ahead of time and then appears on the real screen with a pushSprite() command.

#include "Free_Fonts.h" // The header file attached to this sketch has font nicknames and whatnot.

int VerticalPosition = 40; // Upper edge of scrolling banner.
int ScrollStepCounter = 0;
int ScrollStep = -1; // Must be negative for scrolling from right to left. Use a more neg. number for faster scrolling, but jumpier.
int16_t MsgPixWidth; // The width of your entire message to be scrolled, in pixels.


String messages[] = { "See the child. He is pale and thin, he wears a thin and ragged linen shirt. He stokes the scullery fire.", // Blood Meridian
"She shook him awake into the quiet darkness. Hush, she said. Quit hollerin. He sat up. What? he said. What?", // The Outer Dark
"This then would be Chicago in the winter of the last year of her life.", // The Passenger
"Hi. I'm Dr Cohen. You're not the Dr Cohen I was expecting. Sorry about that. That would be Dr Robert Cohen." }; // Stella Maris
String MessageToScroll = messages[random(4)];
int SpaceBetweenRepeats = 1000; // in pixels.
int TopPadding = 10; // to keep the letters from touching the top edge of banner.
////////////////////////////////////////////////////////////
void setup() {

  tft.init();
  tft.setRotation(1); // for my TTGO (240x135) T-display this makes it landscape mode with the USB port to the right.
  // 0: portrait, USB at bottom
  // 1: Landscape, USB at right
  // 2: portrait, USB at top
  // 3: Landscape, USB at left

  attachInterrupt(0, goToSleep, CHANGE);

  //set screen Back Light brightness
  pinMode(TFT_BL, OUTPUT);
  ledcSetup(0, 5000, 8); // 0-15, 5000, 8
  ledcAttachPin(TFT_BL, 0); // TFT_BL, 0 - 15
  ledcWrite(0, 1); // 0-15, 0-255 (with 8 bit resolution); 0=totally dark;255=totally shiny


  tft.fillScreen(TFT_BLACK);

  tft.setSwapBytes(true); // Color bytes are swapped when writing to RAM, this introduces a small overhead but
                          // there is a net performance gain by using swapped bytes.
  ScrollTextSprite.setFreeFont(FSSB18);  // ESP32 runs out of memory if the font is too big or the message is too long, and just flashes lamely.
  tft.setFreeFont(FSSB18); // In case you want to write non-scrolling text.
  MsgPixWidth = tft.textWidth(MessageToScroll);
  tft.drawString(String(MsgPixWidth), 0,TopPadding, GFXFF); // to see how many pixels wide your message is.
  ScrollTextSprite.setColorDepth(8); // How big a sprite you can use and how fast it moves is greatly influenced by the color depth.
  ScrollTextSprite.createSprite(MsgPixWidth + tft.width(), (tft.fontHeight() + TopPadding)); // Sprite width is display plus the space to allow text to scroll from the right.
  ScrollTextSprite.setTextColor(TFT_WHITE, TFT_BLACK); // Yellow text, black background
  
  delay(1000);
  // Draw it for the first time:
  ScrollTextSprite.drawString(MessageToScroll, tft.width(),TopPadding, GFXFF); // 
  ScrollStepCounter = (MsgPixWidth / abs(ScrollStep)) + SpaceBetweenRepeats;
}

////////////////////////////////////////////////////////////
void loop() {

  tft.drawFloat(BL.getBatteryChargeLevel(), 3, 0, 80, 4);
  delay(10);



  ScrollStepCounter--; 
  if (ScrollStepCounter <= 0)
  { // Redraw the text to the right, just off the screen.
    tft.fillScreen(TFT_BLACK); // to signal the moment it begins the message again.
    delay(200);
    ScrollStepCounter = (MsgPixWidth / abs(ScrollStep))+ SpaceBetweenRepeats;
    ScrollTextSprite.drawString(MessageToScroll, tft.width(),TopPadding, GFXFF);
    tft.fillScreen(TFT_BLACK);
  }
  ScrollTextSprite.scroll(ScrollStep);    // Shift the Sprite over by ScrollStep pixels.
  ScrollTextSprite.pushSprite(0, VerticalPosition); // Upper left corner of sprite

} // end main loop
