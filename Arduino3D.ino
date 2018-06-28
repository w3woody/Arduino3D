/*  3DGraphicsEngine
 *  
 *      Test the 3D graphics engine.
 */

#include "G3D.h"

#if USELIBRARY == 1
#include <SPI.h>       // this is needed for display
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#elif USELIBRARY == 2
#endif

/*
 *  Drawing globals
 */

#if USELIBRARY == 1

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Graphics setup
G3D draw(tft,128,64);

#elif USELIBRARY == 2

Arduboy arduboy;

// Graphics setup
G3D draw(arduboy,128,64);

#endif

// Rotation
static float GXAngle;
static float GYAngle;

void setup() 
{
#if USELIBRARY == 1
    tft.begin();
#elif USELIBRARY == 2
    arduboy.beginNoLogo();
    arduboy.setFrameRate(50);
    arduboy.clear();
    arduboy.display();
#endif
    
    GXAngle = 15;
    GYAngle = 30;
}

void transform()
{
    draw.transformation.setIdentity();
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

#if USELIBRARY == 1
void loop() 
{
    tft.fillScreen(ILI9341_BLACK);

    draw.begin();
    draw.setColor(ILI9341_RED);
    transform();
    drawBox(0,0,0); 
    draw.end();   

    delay(100);
    
    draw.begin();
    draw.setColor(ILI9341_BLACK);
    drawBox(0,0,0); 
    draw.end();   
    
    GXAngle += 0.01;
    GYAngle += 0.02;
}
#elif USELIBRARY == 2
void loop()
{
    if (!arduboy.nextFrame()) return;
    
    arduboy.clear();
    
    draw.begin();
    draw.setColor(WHITE);
    transform();
    drawBox(0,0,0);
    draw.end();
    
    arduboy.display();

    GXAngle += 0.01;
    GYAngle += 0.02;
}
#endif
