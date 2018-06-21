/*  G3D.h
 *
 *      A simple 3D graphics pipeline for the Arduino.
 */

#ifndef _G3D_H
#define _G3D_H

#include <stdint.h>
#include <Adafruit_GFX.h>    // Core graphics library

/********************************************************************/
/*                                                                  */
/*  Math Structures                                                 */
/*                                                                  */
/********************************************************************/

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
                G3D(Adafruit_GFX &lib);
                ~G3D();

		void	setColor(int16_t c)
					{
						color = c;
					}
		
        void    begin();
        void    end();
        void    move(float x, float y);
        void    draw(float x, float y);
    private:
        /*
         *  Internal state
         */
        Adafruit_GFX &lib;
        uint16_t width;
        uint16_t height;
        
        /*
         *	Current drawing color
         */
        
        uint16_t color;
        
        /*
         *	State 2 pipeline; map -1/1 to screen coordinates
         */
        
        void	p2init();
        void	p2movedraw(bool drawFlag, float x, float y);
        void	p2point(float x, float y);
        
        float	p2xsize;		// Width is +/- p2xsize
        float	p2ysize;		// Height is +/- p2ysize
        float	p2scale;		// Assume square pixels.
        float	p2xoff;
        float	p2yoff;

        /*
         *  State 1 pipeline
         */

        bool    p1draw;
        uint16_t p1x;
        uint16_t p1y;
        
        void	p1init();
        void    p1movedraw(bool drawFlag, uint16_t x, uint16_t y);
        void	p1point(uint16_t x, uint16_t y);
};

#endif
