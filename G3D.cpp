/*  G3D.cpp
 *
 *      3D drawing pipeline for the Arduino.
 */

#include "G3D.h"

/********************************************************************/
/*                                                                  */
/*  Constructor/Destructor											*/
/*                                                                  */
/********************************************************************/

/*	G3D::G3D
 *
 *		Construct our pipeline
 */

G3D::G3D(Adafruit_GFX &l) : lib(l)
{
	width = l.width();
	height = l.height();
	
	/* Initialize components of pipeline */
	p1init();
	p2init();
}

G3D::~G3D()
{
}

/********************************************************************/
/*                                                                  */
/*  Begin/End														*/
/*                                                                  */
/********************************************************************/

/*	G3D::begin
 *
 *		Because we do a lot of drawing to draw a screen, we block out
 *	the begin/end calls
 */

void G3D::begin()
{
	lib.startWrite();
}

/*	G3D::end
 *
 *		End writing block
 */

void G3D::end()
{
	lib.endWrite();
}

/********************************************************************/
/*                                                                  */
/*  Move/Draw Support												*/
/*                                                                  */
/********************************************************************/

void G3D::move(float x, float y)
{
	p2movedraw(false,x,y);
}

void G3D::draw(float x, float y)
{
	p2movedraw(true,x,y);
}

void G3D::point(float x, float y)
{
	p2point(x,y);
}

/********************************************************************/
/*                                                                  */
/*  Move/Draw Level 2												*/
/*                                                                  */
/********************************************************************/

/*	G3D::p2init
 *
 *		Initialize p2 framework
 */

void G3D::p2init()
{
	/*
	 *	We subtract one because we want our mapping to work so that
	 *	virtual coordinate -1 is in the middle of the 0th pixel, and
	 *	+1 is in the middle of the width-1 pixel for wide displays.
	 *
	 *	This offset of 1/2 by the pixel width implies we're drawing on
	 *	a display 1 pixel narrower and wider, but then with 1/2 added
	 *	to the pixel coordinate
	 */
	
	uint16_t h1 = height - 1;
	uint16_t w1 = width - 1;
	
	/*
	 *	Calculate the width, height in abstract coordinates. This
	 *	allows me to quickly clip at the clipping level
	 */
	
	if (w1 > h1) {
		p2xsize = 1;
		p2ysize = ((float)h1)/((float)w1);
	} else {
		p2ysize = 1;
		p2xsize = ((float)w1)/((float)h1);
	}
	
	/*
	 *	Calculate the scale, offset to transform virtual to real.
	 *	Note that -1 -> 0 and 1 -> (width-1) or (height-1).
	 */
	
	if (w1 > h1) {
		p2scale = ((float)w1)/2;
	} else {
		p2scale = ((float)h1)/2;
	}
	
	p2xoff = ((float)width)/2;
	p2yoff = ((float)height)/2;
}

/*	p2point
 *
 *		Draw a point at the virtual location on the screen. This
 *	does the appropriate math to scale to our screen coordinates
 */

void G3D::p2point(float x, float y)
{
	// Flip y coordinate so -1 is at bottom
	int16_t xpos = (int16_t)(p2xoff + x * p2scale);
	int16_t ypos = (int16_t)(p2yoff - y * p2scale);
	p1point(xpos,ypos);
}

/*	p2movedraw
 *
 *		Move/draw for virtual coordinates
 */

void G3D::p2movedraw(bool drawFlag, float x, float y)
{
	// Flip y coordinate so -1 is at bottom
	int16_t xpos = (int16_t)(p2xoff + x * p2scale);
	int16_t ypos = (int16_t)(p2yoff - y * p2scale);
	p1movedraw(drawFlag,xpos,ypos);
}

/********************************************************************/
/*                                                                  */
/*  Move/Draw Level 1												*/
/*                                                                  */
/********************************************************************/

/*	p1init
 *
 *		Initialize our pipeline
 */

void G3D::p1init()
{
	p1draw = false;
	p1x = 0;
	p1y = 0;
}

/*	p1movedraw
 *
 *		Level 1 talks directly to the hardware. In our case we talk
 *	directly to the Adafruit GFX library. This is the only point where
 *	we do talk to the GFX library. In theory this could be replaced
 *	with code to simply draw lines using Beshingham's algorithm
 */

void G3D::p1movedraw(bool drawFlag, uint16_t x, uint16_t y)
{
	/*
	 *	We use the p1drawflag and the drawFlag objects to determine the
	 *	way to draw our line. For us, we're always drawing single
	 *	segments, but we theoretically could roll up our lines into a
	 *	collection of line segments and send them on close. (This
	 *	requires hooking G3D::end().)
	 */
	
	if (drawFlag) {
		lib.writeLine(p1x,p1y,x,y,color);
	}
	
	p1draw = drawFlag;
	p1x = x;
	p1y = y;
}

/*	p1point
 *
 *		draw a single pixel point. This does not affect the current pen
 *	location for drawing
 */

void G3D::p1point(uint16_t x, uint16_t y)
{
	lib.writePixel(x,y,color);
}
