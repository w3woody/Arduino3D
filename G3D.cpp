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
	p3init();
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

void G3D::p4movedraw(bool drawFlag, float x, float y, float z)
{
    G3DVector t;

    t.x = transformation.a[0][0] * x + transformation.a[0][1] * y + transformation.a[0][2] * z + transformation.a[0][3];
    t.y = transformation.a[1][0] * x + transformation.a[1][1] * y + transformation.a[1][2] * z + transformation.a[1][3];
    t.z = transformation.a[2][0] * x + transformation.a[2][1] * y + transformation.a[2][2] * z + transformation.a[2][3];
    t.w = transformation.a[3][0] * x + transformation.a[3][1] * y + transformation.a[3][2] * z + transformation.a[3][3];

    p3movedraw(drawFlag,t);
}

void G3D::p4point(float x, float y, float z)
{
    G3DVector t;

    t.x = transformation.a[0][0] * x + transformation.a[0][1] * y + transformation.a[0][2] * z + transformation.a[0][3];
    t.y = transformation.a[1][0] * x + transformation.a[1][1] * y + transformation.a[1][2] * z + transformation.a[1][3];
    t.z = transformation.a[2][0] * x + transformation.a[2][1] * y + transformation.a[2][2] * z + transformation.a[2][3];
    t.w = transformation.a[3][0] * x + transformation.a[3][1] * y + transformation.a[3][2] * z + transformation.a[3][3];

    p3point(t);
}

/********************************************************************/
/*                                                                  */
/*  Move/Draw Level 3												*/
/*                                                                  */
/********************************************************************/

/*	OutCode
 *
 *		Calculate the outcode; this is 1 bit per the six possible clipping
 *	walls on our cubic boundary. Note we clip against w; see blog post
 *	for detailed explanation
 */

static uint8_t OutCode(const G3DVector &v)
{
    uint8_t m = 0;

    if (v.x < -v.w) m |= 1;
    if (v.x > v.w) m |= 2;
    if (v.y < -v.w) m |= 4;
    if (v.y > v.w) m |= 8;
    if (v.z < -v.w) m |= 16;
    if (v.z > 0) m |= 32;

    return m;
}

/*	Lerp
 *
 *		Perform linear interpolation; given two values (a and b) and
 *	an alpha value from 0 to 1, this will compute a new value c.
 *
 *		This is used to find the intersection of the vector with a wall
 */

static void Lerp(const G3DVector &a, const G3DVector &b, float alpha, G3DVector &c)
{
    float a1 = 1.0f - alpha;
    c.x = a1 * a.x + alpha * b.x;
    c.y = a1 * a.y + alpha * b.y;
    c.z = a1 * a.z + alpha * b.z;
    c.w = a1 * a.w + alpha * b.w;
}

/*	G3D::p3init 
 *
 *		Initialize
 */

void G3D::p3init()
{
	p3pos.x = 0;
	p3pos.y = 0;
	p3pos.z = 0;
	p3pos.w = 1;
	p3outcode = 0;
}

/*	G3D::p3point
 *
 *		Draw the point as specified. We do the division necessary to
 *	convert to view space. We simply test to make sure our point is in
 *	the viewspace cube, and plot it if it is.
 */

void G3D::p3point(const G3DVector &v)
{
	if (!OutCode(v)) {
		p2point(v.x/v.w, v.y/v.w);
	}
}

/*	G3D::p3init
 *
 *		Initialize
 */

void G3D::p3movedraw(bool drawFlag, const G3DVector &v)
{
    uint8_t newOutCode = OutCode(v);
    G3DVector lerp;
    if (drawFlag) {
        uint8_t mask = newOutCode | p3outcode;

        /*
         *  Fast accept/reject
         */

        if (0 == (newOutCode & p3outcode)) {
            if (0 == mask) {
                // Fast accept. Both points are inside; we assume
                // the previous point was already passed upwards, 
                // so we only draw to the current vector location
                p2movedraw(true,v.x/v.w,v.y/v.w);
            } else {
                // At this point we have a line that crosses
                // a boundary. We calculate the alpha between
                // 0 and 1 for each point along the line.
                //
                // (This is the Liang-Barsky optimization of
                // the Cohen-Sutherland algorithm)

                float aold = 0;     // (1 - alpha) * old + alpha * new = v
                float anew = 1;     // in the above, 0 == old, 1 == new.
                float alpha;

                uint8_t m = 1;
                uint8_t i;
                for (i = 0; i < 6; ++i) {
                    if (mask & m) {
                        // Calculate alpha; the intersection along the line
                        // vector intersecting the specified edge
                        // 
                        // These are specific cases of the general equation
                        // alpha = (c - old)/(new - old), which yields
                        // alpha == 0 if c == old, and alpha == 1 if c == new,
                        // and with alpha as a linear scale with the intersection
                        // point sliding from old to new.

                        switch (i) {
                            default:
                            case 0:         // clip (1,0,0,1)
                                alpha = p3pos.x + p3pos.w;
                                alpha = alpha/(alpha - (v.x + v.w));
                                break;
                            case 1:         // clip (1,0,0,-1)
                                alpha = - p3pos.x + p3pos.w;
                                alpha = alpha/(alpha - (v.x - v.w));
                                break;
                            case 2:         // clip (0,1,0,1)
                                alpha = p3pos.y + p3pos.w;
                                alpha = alpha/(alpha - (v.y + v.w));
                                break;
                            case 3:         // clip (0,1,0,-1)
                                alpha = - p3pos.y + p3pos.w;
                                alpha = alpha/(alpha - (v.y - v.w));
                                break;
                            case 4:         // clip (0,0,1,1)
                                alpha = p3pos.z + p3pos.w;
                                alpha = alpha/(alpha - (v.z + v.w));
                                break;
                            case 5:         // clip (0,0,1,0)
                                alpha = p3pos.z;
                                alpha = alpha/(alpha - v.z);
                                break;
                        }

                        if (p3outcode & m) {
                            if (aold < alpha) aold = alpha;
                        } else {
                            if (anew > alpha) anew = alpha;
                        }

                        if (aold > anew) {
                            // We have a case where the line is not visible
                            // because it's outside the visible frustrum.
                            // abort.
                            break;
                        }
                    }
                    m <<= 1;
                }

                if (i >= 6) {
                    // Ran all clipping edges.
                    if (p3outcode) {
                        Lerp(p3pos,v,aold,lerp);
						p2movedraw(false,lerp.x/lerp.w,lerp.y/lerp.w);
                    }

                    // Draw to the new point
                    if (newOutCode) {
                        Lerp(p3pos,v,anew,lerp);
						p2movedraw(true,lerp.x/lerp.w,lerp.y/lerp.w);
                    } else {
						p2movedraw(true,v.x/v.w,v.y/v.w);
                    }
                }
            }
        }
    } else {
        if (newOutCode == 0) {
			p2movedraw(false,v.x/v.w,v.y/v.w);
        }
    }

    p3outcode = newOutCode;
    p3pos = v;
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
		p2xsize = ((float)w1)/((float)h1);
		p2ysize = 1;
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
