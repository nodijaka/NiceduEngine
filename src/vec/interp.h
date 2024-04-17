

#ifndef INTERP_H
#define INTERP_H

#include "vec.h"
#include "quat.h"

//
// todo: Hermite spline
//

namespace linalg {
    
    template<typename T>
    inline T lerp(const T &a, const T &b, float x) { return a*(1.0f-x) + b*x; }
    
    template<typename T>
    inline T smoothstep(const T &x, const T &a, const T &b)
    {
        if (x < a)
            return 0.0;
        if (x >= b)
            return 1.0;
        
        T t = (x-a)/(b-a);
        return t*t*(3.0-2.0*t);
    }
    
    //
    // Binary step & ramp functions
    //
    
    // Right-continuous step-up, f(a)=1
    inline int stepr(float a, float x) { return (int)(x >= a); }
    // Left-continuous step-up, f(a)=0
    inline int stepl(float a, float x) { return (int)(x > a); }
    
    inline int step_up(float a, float x) { return stepr(a, x); }
    inline int step_down(float a, float x) { return 1-stepl(a, x); }
    inline int step_updown(float a, float b, float x) { return stepr(a, x) - stepl(b, x); }
    
    template<int A>
    inline float ramp(float a, float b, float x) { return 1; }
    
    // TODO: general Catmull-Rom interpolation, e.g. Texturing & Modeling, page 31
    //
//    template<typename T>
//    T spline(float x, T *knots, const int &nbr_knots, const float &tension = 0.5f)
//    {
//        assert(nbr_knots >= 4);
//        
//        if (x > 1.0f)
//            x = fmodf(x, 1.0f);
//        
//        //    ...
//        
//        return knots[0];
//    }
    
    //
    // Linear interpolation
    //
    // p(x) =   | 1  x | *  |  1  0 | * | p0 |
	//                      | -1  1 |	| p1 |
    //
    inline vec3f evalLERP(const vec3f &p0,
                   const vec3f &p1, float x)
    {
        return	p0 * ( 1.0f - x ) + p1 * ( x );
    }
    
    
    //
    // Catmull-Rom spline interpolation
    //
    //                                | 0	1	0      0 |   | p0 |
    // Q(x) =   | 1  x   x^2  x^3 | * | -t	0	t	   0 | * | p1 |
    //                                | 2t	t-3	3-2t  -t |	 | p2 |
    //                                | -t	2-t	t-2    t |	 | p3 |
    //
    inline vec3f evalCatmullRom(const vec3f &p0,
                         const vec3f &p1,
                         const vec3f &p2,
                         const vec3f &p3,
                         float t,
                         float x)
    {
        float	x2 = x*x, x3 = x2 * x;
        
        vec3f Q = (	p0 * ( -t*x + 2.0f*t*x2 - t*x3 ) +
                   p1 * ( 1.0f + (t-3.0f)*x2 + (2.0f-t)*x3 ) +
                   p2 * ( t*x + (3.0f-2.0f*t)*x2 + (t-2.0f)*x3 ) +
                   p3 * ( -t*x2 + t*x3 ) );
        
        return	Q;
    }
    
    
    //
    // Catmull-Rom tangent
    //
    //          dQ(x)
    // T(x) =	-----
    //           dx
    //
    inline vec3f evalCatmullRomTangent(const vec3f &p0,
                                const vec3f &p1,
                                const vec3f &p2,
                                const vec3f &p3,
                                float t,
                                float x)
    {
        float	x2 = x*x;
        
        vec3f T = (	p0 * ( -t + 4.0f*t*x - 3.0f*t*x2 ) +
                   p1 * ( 2.0f*(t-3.0f)*x + 3.0f*(2.0f-t)*x2 ) +
                   p2 * ( t + 2.0f*(3.0f-2.0f*t)*x + 3.0f*(t-2.0f)*x2 ) +
                   p3 * ( -2.0f*t*x + 3.0f*t*x2 ) );
        
        return	T;
    }
    
    
    //
    // Cubic Bézier spline interpolation
    //
    //          |	1	-3	3	-1	|		|	1	|		|	p0	|
    // B(x) =	|	0	3	-6	3	|	*	|	x	|	*	|	p1	|
    //          |	0	0	3	-3	|		|	x^2	|		|	p2	|
    //          |	0	0	0	1	|		|	x^3	|		|	p3	|
    //
    inline vec3f evalBezier(const vec3f &p0,
                     const vec3f &p1,
                     const vec3f &p2,
                     const vec3f &p3,
                     float x)
    {
        float	x2 = x*x, x3 = x2 * x;
        
        vec3f B =	p0 * (1.0f - 3.0f*x + 3.0f*x2 - x3) +
                    p1 * (3.0f*x - 6.0f*x2 + 3.0f*x3) +
                    p2 * (3.0f*x2 - 3.0f*x3) +
                    p3 * (x3);
        
        return	B;
    }
    
    
    //
    // Cubic Bézier tangent
    //
    //          dB(x)
    // T(x) =	-----
    //           dx
    //
    inline vec3f evalBezierTangent(const vec3f &p0,
                            const vec3f &p1,
                            const vec3f &p2,
                            const vec3f &p3,
                            float x)
    {
        float	x2 = x*x;
        
        vec3f T =	p0 * (-3.0f + 6.0f*x - 3.0f*x2) +
                    p1 * (3.0f - 12.0f*x + 9.0f*x2) +
                    p2 * (6.0f*x - 9.0f*x2) +
                    p3 * (3.0f*x2);
        
        return	T;
    }
    
    //
    // Hermite interpolation
    //
    // Interpolates between p0 and p1, based on tangents t0 and t1
    // Runs through all control points, unlike e.g. Catmull Rom interpolation
    // https://msdn.microsoft.com/en-us/library/windows/desktop/bb205509%28v=vs.85%29.aspx
    //
    //
    inline vec3f evalHermite(const vec3f &p0,
                     const vec3f &p1,
                     const vec3f &t0,
                     const vec3f &t1,
                     float x)
    {
        float x2 = x*x, x3 = x2 * x;
        
        vec3f Q =   p0 * (2.0f*x3 - 3.0f*x2 + 1.0f) +
                    p1 * (-2.0f*x3 + 3.0f*x2) +
                    t0 * (x3 - 2.0f*x2 + x) +
                    t1 * (x3 - x2);
        return Q;
    }
    
    //
    // Todo
    //
    // SLERP
    // Spherical Linear Interpolation
    //
    // Slerp(v0, v1, t) =   v0 * sin[(1-t)a]/sin a +
    //                      v1 * sin[t*a]/sin a
    //
    // where,   v0 and v1 are unit vectors
    //          a is the angle between p0 and p1: cos a = p0.p1
    //
    // Martin Baker, http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
    // Wikipedia, https://en.wikipedia.org/wiki/Slerp
    //
    template<typename T>
    inline vec3<T> slerp(vec3<T>& v0, vec3<T>& v1, float t )
    {
        
    }
    
    //
    // nLERP - Normalized quaternion lerp
    //
    // Adopted from [1].
    // J. Blow recommends using nlerp over quaternion slerp (qslerp) [1,2].
    // Compared to qslerp, nlerp is faster and commutative. It is not
    // constant in velocity, although this is not as important in most cases.
    //
    // [1] http://physicsforgames.blogspot.com/2010/02/quaternions.html
    // [2] Hacking Quaternions
    // http://number-none.com/product/Hacking%20Quaternions/
    // [3] Understanding Slerp, Then Not Using It
    // http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
    //
    template<typename T>
    inline quat<T> qnlerp(const quat<T>& q0, const quat<T>& q1, float t)
    {
        quat<T> result;
        float dot = q0.qw*q1.qw + q0.qx*q1.qx + q0.qy*q1.qy + q0.qz*q1.qz;
        float ti = static_cast<T>(1.0f) - t;
        if(dot < static_cast<T>(0.0f))
        {
            quat<T> qtmp;
            qtmp.qw = -q1.qw;
            qtmp.qx = -q1.qx;
            qtmp.qy = -q1.qy;
            qtmp.qz = -q1.qz;
            result.qw = ti*q0.qw + t*qtmp.qw;
            result.qx = ti*q0.qx + t*qtmp.qx;
            result.qy = ti*q0.qy + t*qtmp.qy;
            result.qz = ti*q0.qz + t*qtmp.qz;
        }
        else
        {
            result.qw = ti*q0.qw + t*q1.qw;
            result.qx = ti*q0.qx + t*q1.qx;
            result.qy = ti*q0.qy + t*q1.qy;
            result.qz = ti*q0.qz + t*q1.qz;
        }
        result.normalize();
        return result;
    }
    
    //
    // Todo
    //
    // Quaternion SLERP
    // Spherical Linear Interpolation
    //
    // Slerp(q0, q1, t) = (q1 q0^-1)^t q0
    //
    // Martin Baker, http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
    // Wikipedia, https://en.wikipedia.org/wiki/Slerp
    //
    template<typename T>
    inline quat<T> qslerp(quat<T>& q0, quat<T>& q1, float t)
    {
        /*
         // assimp
         
         // ---------------------------------------------------------------------------
         // Performs a spherical interpolation between two quaternions
         // Implementation adopted from the gmtl project. All others I found on the net fail in some cases.
         // Congrats, gmtl!
         template<typename TReal>
         inline void aiQuaterniont<TReal>::Interpolate( aiQuaterniont& pOut, const aiQuaterniont& pStart, const aiQuaterniont& pEnd, TReal pFactor)
         {
         // calc cosine theta
         TReal cosom = pStart.x * pEnd.x + pStart.y * pEnd.y + pStart.z * pEnd.z + pStart.w * pEnd.w;
         
         // adjust signs (if necessary)
         aiQuaterniont end = pEnd;
         if( cosom < static_cast<TReal>(0.0))
         {
         cosom = -cosom;
         end.x = -end.x;   // Reverse all signs
         end.y = -end.y;
         end.z = -end.z;
         end.w = -end.w;
         }
         
         // Calculate coefficients
         TReal sclp, sclq;
         if( (static_cast<TReal>(1.0) - cosom) > static_cast<TReal>(0.0001)) // 0.0001 -> some epsillon
         {
         // Standard case (slerp)
         TReal omega, sinom;
         omega = std::acos( cosom); // extract theta from dot product's cos theta
         sinom = std::sin( omega);
         sclp  = std::sin( (static_cast<TReal>(1.0) - pFactor) * omega) / sinom;
         sclq  = std::sin( pFactor * omega) / sinom;
         } else
         {
         // Very close, do linear interp (because it's faster)
         sclp = static_cast<TReal>(1.0) - pFactor;
         sclq = pFactor;
         }
         
         pOut.x = sclp * pStart.x + sclq * end.x;
         pOut.y = sclp * pStart.y + sclq * end.y;
         pOut.z = sclp * pStart.z + sclq * end.z;
         pOut.w = sclp * pStart.w + sclq * end.w;
         }
         */
        
        /*
         // Baker
         
         quat slerp(quat qa, quat qb, double t) {
         // quaternion to return
         quat qm = new quat();
         // Calculate angle between them.
         double cosHalfTheta = qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z;
         // if qa=qb or qa=-qb then theta = 0 and we can return qa
         if (abs(cosHalfTheta) >= 1.0){
         qm.w = qa.w;qm.x = qa.x;qm.y = qa.y;qm.z = qa.z;
         return qm;
         }
         // Calculate temporary values.
         double halfTheta = acos(cosHalfTheta);
         double sinHalfTheta = sqrt(1.0 - cosHalfTheta*cosHalfTheta);
         // if theta = 180 degrees then result is not fully defined
         // we could rotate around any axis normal to qa or qb
         if (fabs(sinHalfTheta) < 0.001){ // fabs is floating point absolute
         qm.w = (qa.w * 0.5 + qb.w * 0.5);
         qm.x = (qa.x * 0.5 + qb.x * 0.5);
         qm.y = (qa.y * 0.5 + qb.y * 0.5);
         qm.z = (qa.z * 0.5 + qb.z * 0.5);
         return qm;
         }
         double ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
         double ratioB = sin(t * halfTheta) / sinHalfTheta;
         //calculate Quaternion.
         qm.w = (qa.w * ratioA + qb.w * ratioB);
         qm.x = (qa.x * ratioA + qb.x * ratioB);
         qm.y = (qa.y * ratioA + qb.y * ratioB);
         qm.z = (qa.z * ratioA + qb.z * ratioB);
         return qm;
         }
         */
        return quat<T>();
    }
    
} // namespace linalg

#endif
