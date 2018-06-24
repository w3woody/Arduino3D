/*  3DGraphicsEngine
 *  
 *      Test the 3D graphics engine.
 */

#include <SPI.h>       // this is needed for display
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include "G3D.h"

/*
 *  Drawing globals
 */

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Graphics setup
G3D draw(tft);

void setup() 
{
    tft.begin();
}

void drawBox()
{
    draw.move(-0.5, 0.5, 0.5);
    draw.draw(-0.5,-0.5, 0.5);
    draw.draw( 0.5,-0.5, 0.5);
    draw.draw( 0.5, 0.5, 0.5);
    draw.draw(-0.5, 0.5, 0.5);
}

void loop() 
{
    tft.fillScreen(ILI9341_BLACK);

    draw.begin();
    draw.setColor(ILI9341_RED);
    drawBox();
    draw.end();   
    
    for (;;) ;
}
