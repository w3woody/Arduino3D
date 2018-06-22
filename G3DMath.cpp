/*  G3DMath.cpp
 *
 *      Math routines for handling 3D homogeneous coordinates
 */

#include <math.h>
#include <stdint.h>
#include "G3DMath.h"

/************************************************************************/
/*                                                                      */
/*  Matrix Routines                                                     */
/*                                                                      */
/************************************************************************/

/*  G3DMatrix::G3DMatrix
 *   
 *      Construct. This initializes with setIdentity
 */

G3DMatrix::G3DMatrix()
{
    setIdentity();
}

/************************************************************************/
/*                                                                      */
/*  Matrix Creation                                                     */
/*                                                                      */
/************************************************************************/

/*  G3DMatrix::setIdentity
 *   
 *      Set to identity matrix
 */

void G3DMatrix::setIdentity()
{
    for (uint8_t i = 0; i < 4; ++i) {
        for (uint8_t j = 0; j < 4; ++j) {
            a[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

/*  G3DMatrix::setTranslate
 *   
 *      Create a translation matrix
 */

void G3DMatrix::setTranslate(float x, float y, float z)
{
    setIdentity();
    a[0][3] = x;
    a[1][3] = y;
    a[2][3] = z;
}

/*  G3DMatrix::setScale
 *   
 *      Scale matrix
 */

void G3DMatrix::setScale(float x, float y, float z)
{
    setIdentity();
    a[0][0] = x;
    a[1][1] = y;
    a[2][2] = z;
}

/*  G3DMatrix::setScale
 *   
 *      Scale matrix
 */

void G3DMatrix::setScale(float s)
{
    setIdentity();
    a[0][0] = s;
    a[1][1] = s;
    a[2][2] = s;
}

/*  G3DMatrix::setRotate
 *   
 *      Rotational matrix. Axis is 0 (x), 1 (y) or 2 (z)
 */

void G3DMatrix::setRotate(uint8_t axis, float angle)
{
    setIdentity();
    float c = cos(angle);
    float s = sin(angle);
    
    switch (axis) {
        case AXIS_X:
            a[1][1] = c;
            a[2][2] = c;
            a[1][2] = -s;
            a[2][1] = s;
            break;
        case AXIS_Y:
            a[0][0] = c;
            a[2][2] = c;
            a[0][2] = s;
            a[2][0] = -s;
            break;
        case AXIS_Z:
            a[0][0] = c;
            a[1][1] = c;
            a[0][1] = s;
            a[1][0] = -s;
            break;
        default:
            return;
    }
}

/*  G3DMatrix::setPerspective
 *   
 *      Set the perspective matrix. See https://github.com/w3woody/clipping
 */

void G3DMatrix::setPerspective(float fov, float near)
{
    setIdentity();

    a[0][0] = fov;
    a[1][1] = fov;
    a[2][2] = 0.0f;
    a[3][3] = 0.0f;
    a[2][3] = -1.0f;
    a[3][2] = -near;
}


/************************************************************************/
/*                                                                      */
/*  Matrix Math                                                         */
/*                                                                      */
/************************************************************************/

/*  G3DMatrix::multiply
 *   
 *      Multiply the provided matrix into this one. This is done via pre-
 *  multiplication, and allows us to chain transformations. The way this
 *  is done implies you must start with the last (perspective) matrix first,
 *  pushing earlier transformations later. This allows you (if you wish)
 *  to build a push stack of matrices, though in the limited confines of
 *  an embedded processor that stack cannot be very large...
 */

void G3DMatrix::multiply(const G3DMatrix &m)
{
    float tmp[4];
    float n;
    
    for (uint8_t i = 0; i < 4; ++i) {
        /*
         * Comlumn by column multiply and replace.
         */
        for (uint8_t j = 0; j < 4; ++j) {
            n = 0.0f;
            for (uint8_t k = 0; k < 4; ++k) {
                n += a[i][k] * m.a[k][j];
            }
            tmp[j] = n;
        }

        for (uint8_t j = 0; j < 4; ++j) {
            a[i][j] = tmp[j];
        }
    }
}

/************************************************************************/
/*                                                                      */
/*  Vector multiplication                                               */
/*                                                                      */
/************************************************************************/

/*  G3DVector:multiply
 *
 *      Set this vector to the multiplcation of the provided vector and
 *  matrix
 */

void G3DVector::multiply(const G3DMatrix &m, const G3DVector &v)
{
    x = m.a[0][0] * v.x + m.a[0][1] * v.y + m.a[0][2] * v.z + m.a[0][3] * v.w;
    y = m.a[1][0] * v.x + m.a[1][1] * v.y + m.a[1][2] * v.z + m.a[1][3] * v.w;
    z = m.a[2][0] * v.x + m.a[2][1] * v.y + m.a[2][2] * v.z + m.a[2][3] * v.w;
    w = m.a[3][0] * v.x + m.a[3][1] * v.y + m.a[3][2] * v.z + m.a[3][3] * v.w;
}
