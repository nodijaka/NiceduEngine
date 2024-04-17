
/*
 quarernion
 v 1.0
 
 Carl Johan Gribel (c) 2011, cjgribel@gmail.com
 
 templated definitions in cpp-file requires explicit template specialisation (<float> provided here):
 http://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file
 */

#include "quat.h"
#include "mat.h"
#include "vec.h"

namespace linalg
{
    // -------------------------------------------------------------------------
    // Quaternion from (rotation) matrix R
    // -------------------------------------------------------------------------
    // Note:    Extension for case when trace Tr(R) <= 0 was
    //          inspired by Martin Baker [1].
    //          This occurs e.g. for a rotation of pi radians around (0,1,0).
    //
    // Note:    For a correct conversion, the provided rotation matrix
    //          has to be 'special orthogonal' <=> Lie group SO(3):
    //              R*R^T  = I, det(R) = +/- 1  (orthogonal)
    //              R*R^T  = I, det(R) = +1     (special orthogonal)
    //          All 3x3 rotations around the origin are SO(3) [3].
    //
    // -------------------------------------------------------------------------
    // [1] http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
    // [2] http://mathworld.wolfram.com/SpecialOrthogonalMatrix.html
    // [3] https://en.wikipedia.org/wiki/Rotation_group_SO(3)
    // -------------------------------------------------------------------------
    template<class T>
    quat<T>::quat(const mat3<T> &R)
    {
        T Tr = R.m11+R.m22+R.m33;
        
        if (Tr > 0)
        {
            qw = 0.5*sqrt(1.0+Tr);
            float f = 0.25/qw;
            qx = f*(R.m32-R.m23);
            qy = f*(R.m13-R.m31);
            qz = f*(R.m21-R.m12);
        }
        else if (R.m11 > R.m22 && R.m11 > R.m33)
        {
            qx = 0.5*sqrt(1.0+R.m11-R.m22-R.m33);
            float f = 0.25/qx;
            qy = f*(R.m12+R.m21);
            qz = f*(R.m13+R.m31);
            qw = f*(R.m32-R.m23);
        }
        else if (R.m22 > R.m11 && R.m22 > R.m33)
        {
            qy = 0.5*sqrt(1.0+R.m22-R.m11-R.m33);
            float f = 0.25/qy;
            qz = f*(R.m23+R.m32);
            qw = f*(R.m13-R.m31);
            qx = f*(R.m12+R.m21);
        }
        else
        {
            qz = 0.5*sqrt(1.0+R.m33-R.m11-R.m22);
            float f = 0.25/qz;
            qw = f*(R.m21-R.m12);
            qx = f*(R.m13+R.m31);
            qy = f*(R.m23+R.m32);
        }
        
//         Alt. branchless version [1]
//         quaternion.w = sqrt( max( 0, 1 + m00 + m11 + m22 ) ) / 2;
//         quaternion.x = sqrt( max( 0, 1 + m00 - m11 - m22 ) ) / 2;
//         quaternion.y = sqrt( max( 0, 1 - m00 + m11 - m22 ) ) / 2;
//         quaternion.z = sqrt( max( 0, 1 - m00 - m11 + m22 ) ) / 2;
//         Q.x = _copysign( Q.x, m21 - m12 )
//         Q.y = _copysign( Q.y, m02 - m20 )
//         Q.z = _copysign( Q.z, m10 - m01 )
    }
    // explicit template specialisation for <float>
    template quat<float>::quat(const mat3<float> &m);
    
    
    template<class T>
    quat<T>::quat(const float &theta, const vec3<T> &e)
    {
        float s = sin(theta/2.0);
        qw = cos(theta/2.0);
        qx = e.x*s;
        qy = e.y*s;
        qz = e.z*s;
    }
    template quat<float>::quat(const float &theta, const vec3<float> &e);
    
    
    template<class T>
    T quat<T>::getEulerAngle() const
    {
        return 2.0*acos(qw);
    }
    template float quat<float>::getEulerAngle() const;
    
    
    template<class T>
    vec3<T> quat<T>::getEulerAxis(const T &theta) const
    {
        vec3<T> e = vec3<T>(qx, qy, qz);
        e.normalize();
        return e;
    }
    template vec3<float> quat<float>::getEulerAxis(const float &theta) const;
    
    //
    // Time-derivative dQ/dt for a given Q (this instance)
    // and a velocity vector w (provided)
    //
    // dQ/dt = 0.5 * [0, w]' * Q (quaternion mult)
    //
    // http://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions#Quaternion_.E2.86.94_angular_velocities
    //
    template<class T>
    quat<T> quat<T>::getQdot(const vec3<T> &w) const
    {
        return (quat<T>(0, w.x, w.y, w.z)*0.5) * *this;
    }
    template quat<float> quat<float>::getQdot(const vec3<float> &w) const;
    
    //
    // (...the other way around)
    // Velocity vector w from a given time-
    // derivative dQ/dt (this instance) and Q (provided)
    //
    // w = 2 * qQ/dt * Q^-1
    //
    // http://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions#Quaternion_.E2.86.94_angular_velocities
    //
    template<class T>
    vec3<T> quat<T>::get_W_from_Qdot(const quat<T> &Q) const
    {
        quat<T> Qdot = *this;
        quat<T> Wq = (Qdot * 2.0) * Q.inverse();
        vec3<T> W = vec3<T>(Wq.qx, Wq.qy, Wq.qz);
        
        return W;
    }
    template vec3<float> quat<float>::get_W_from_Qdot(const quat<float> &Q) const;
    
    //
    // Decompose/factorize rotation quaternion wrt to a vector u into
    // twist: rotation around u
    // swing: rotation around vector perpendicular to u
    //
    // The original rotation can be recovered via q = swing * twist
    //
    // References
    // http://stackoverflow.com/questions/3684269/component-of-a-quaternion-rotation-around-an-axis
    // http://www.euclideanspace.com/maths/geometry/rotations/for/decomposition/
    //
    template<class T>
    void quat<T>::twistswing_decomposition(const vec3<T> &u, quat<T> &twist, quat<T> &swing) const
    {
        vec3<T> r = vec3<T>(qx, qy, qz);
        vec3<T> p = r.project(u);
        twist = quat<T>(qw, p.x, p.y, p.z);
        twist.normalize();
        swing = *this * twist.conjugate();
    }
    template void quat<float>::twistswing_decomposition(const vec3<float> &u, quat<float> &twist, quat<float> &swing) const;
    
}
