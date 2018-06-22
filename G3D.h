/*  G3D.h
 *
 *      A simple 3D graphics pipeline for the Arduino.
 */

#ifndef _G3D_H
#define _G3D_H

#include <stdint.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include "G3DMath.h"

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
        
        G3DMatrix transformation;
        
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
        float	p2scale;		// coordinate transform scale.
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
