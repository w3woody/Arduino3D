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

// Rotation
static float GXAngle;
static float GYAngle;

void setup() 
{
    tft.begin();
    
    GXAngle = 15;
    GYAngle = 30;
}

void transform()
{
    draw.perspective(1.0f,0.5f);
    draw.translate(0,0,-6);
    draw.rotate(AXIS_X,GXAngle);
    draw.rotate(AXIS_Y,GYAngle);
}

void drawBox(int x, int y, int z)
{
    draw.move(x-1,y-1,z-1);
    draw.draw(x+1,y-1,z-1);
    draw.draw(x+1,y+1,z-1);
    draw.draw(x-1,y+1,z-1);
    draw.draw(x-1,y-1,z-1);
    draw.draw(x-1,y-1,z+1);
    draw.draw(x+1,y-1,z+1);
    draw.draw(x+1,y+1,z+1);
    draw.draw(x-1,y+1,z+1);
    draw.draw(x-1,y-1,z+1);
    draw.move(x+1,y-1,z-1);
    draw.draw(x+1,y-1,z+1);
    draw.move(x+1,y+1,z-1);
    draw.draw(x+1,y+1,z+1);
    draw.move(x-1,y+1,z-1);
    draw.draw(x-1,y+1,z+1);
}

void loop() 
{
    tft.fillScreen(ILI9341_BLACK);

    draw.begin();
    draw.setColor(ILI9341_RED);
    transform();
    drawBox(0,0,0); 
    draw.end();   
    
    GXAngle += 0.01;
    GYAngle += 0.02;
    
    for (;;) ;
}
