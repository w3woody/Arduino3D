/*  G3DMath.h
 *
 *      Math routines
 */
 
#ifndef _G3DMATH_H
#define _G3DMATH_H

/************************************************************************/
/*                                                                      */
/*  Matrix Structures                                                   */
/*                                                                      */
/************************************************************************/

#define AXIS_X          0
#define AXIS_Y          1
#define AXIS_Z          2

/*  G3DMatrix
 *
 *      A 3D matrix. THis is a 64 byte object which stores the full
 *  3D representation of the transformation matrix
 */

class G3DMatrix
{
    public:
                        G3DMatrix();

        // Initialize matrix
        void            setIdentity();
        void            setTranslate(float x, float y, float z);
        void            setScale(float x, float y, float z);
        void            setScale(float x);
        void            setRotate(uint8_t axis, float angle);
        void            setPerspective(float fov, float near);

        // Inline multiply transformation matrix
        void            multiply(const G3DMatrix &m);
        
        // Raw contents of the matrix
        float           a[4][4];
};

/*	G3DVector
 *
 *		A homogeneous vector.
 */

struct G3DVector {
	float x;
	float y;
	float z;
	float w;

    // Math support
    void                multiply(const G3DMatrix &m, const G3DVector &v);
};

#endif // _G3DMATH_H
