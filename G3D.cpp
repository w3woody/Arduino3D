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

void G3D::move(uint16_t x, uint16_t y)
{
	p1movedraw(false,x,y);
}

void G3D::draw(uint16_t x, uint16_t y)
{
	p1movedraw(true,x,y);
}

void G3D::point(uint16_t x, uint16_t y)
{
	p1point(x,y);
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
