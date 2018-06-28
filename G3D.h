/*  G3D.h
 *
 *      A simple 3D graphics pipeline for the Arduino.
 */

#ifndef _G3D_H
#define _G3D_H

/********************************************************************/
/*                                                                  */
/*  Globals														    */
/*                                                                  */
/********************************************************************/

#define USELIBRARY			2	// 1 = Adafruit, 2 = Arduboy

#include <stdint.h>
#include "G3DMath.h"

#if USELIBRARY == 1
#include <Adafruit_GFX.h>    // Core graphics library
#elif USELIBRARY == 2
#include <Arduboy.h>
#endif

/********************************************************************/
/*                                                                  */
/*  G3D class, requires reference to GFX library and screen size    */
/*                                                                  */
/********************************************************************/

/*  G3D
 *
 *      A simple 3D drawing pipeline
 */

class G3D
{
    public:
#if USELIBRARY == 1
                G3D(Adafruit_GFX &lib);
#elif USELIBRARY == 2
                G3D(Arduboy &lib, uint16_t width, uint16_t height);
#endif
                ~G3D();

#if USELIBRARY == 1
		void	setColor(uint16_t c)
					{
						color = c;
					}
#elif USELIBRARY == 2
		void	setColor(uint8_t c)
					{
						color = c;
					}
#endif
	
        void    begin();
        void    end();
        void    move(float x, float y, float z)
        			{
        				p4movedraw(false,x,y,z);
        			}
        void    draw(float x, float y, float z)
        			{
        				p4movedraw(true,x,y,z);
        			}
        void    point(float x, float y, float z)
        			{
        				p4point(x,y,z);
        			}
                
        void	translate(float x, float y, float z);
        void	scale(float x, float y, float z);
        void	scale(float s);
        void    rotate(uint8_t axis, float angle);
        void	perspective(float fov, float nclip);
        void	orthographic(void);
        
        G3DMatrix transformation;
    private:
        /*
         *  Internal state
         */
#if USELIBRARY == 1
        Adafruit_GFX &lib;
#elif USELIBRARY == 2
        Arduboy &lib;
#endif

        uint16_t width;
        uint16_t height;
        
        /*
         *	Current drawing color
         */
        
#if USELIBRARY == 1
        uint16_t color;
#elif USELIBRARY == 2
        uint8_t color;
#endif
      
        /*
         *	Stage 4 pipeline; 3D transformation
         */
        
        void	p4point(float x, float y, float z);
        void	p4movedraw(bool drawFlag, float x, float y, float z);
        
        /*
         *	Stage 3 pipeline; 3D clipping engine
         */
        
        G3DVector p3pos;
        uint8_t	p3outcode;
        
        void	p3init();
        void	p3movedraw(bool drawFlag, const G3DVector &v);
        void	p3point(const G3DVector &v);
        
        /*
         *	Stage 2 pipeline; map -1/1 to screen coordinates
         */
        
        void	p2init();
        void	p2movedraw(bool drawFlag, float x, float y);
        void	p2point(float x, float y);
        
        float	p2xsize;		// viewport width +/-
        float	p2ysize;		// viewport height +/-
        float	p2xscale;		// coordinate transform scale.
        float	p2yscale;
        float	p2xoff;			// coordinate transform offset
        float	p2yoff;

        /*
         *  Stage 1 pipeline
         */

        bool    p1draw;
        uint16_t p1x;
        uint16_t p1y;
        
        void	p1init();
        void    p1movedraw(bool drawFlag, uint16_t x, uint16_t y);
        void	p1point(uint16_t x, uint16_t y);
};

#endif // _G3D_H
