
/*
 * Tau3D Dynamics
 * Carl Johan Gribel (c) 2011, cjgribel@gmail.com
 *
 */

#include "mat.h"
#include "vec.h"

/*
 templated definitions in cpp-file requires explicit template specialisation
 
 http://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file
 */

namespace linalg
{
    
    template <class T>
    vec2<T> mat2<T>::operator*(const vec2<T>& rhs) const
    {
        return vec2<T>(m11*rhs.x + m12*rhs.y, m21*rhs.x + m22*rhs.y);
    }
    // explicit template specialisation for <float>
    template vec2<float> mat2<float>::operator*(const vec2<float>& rhs) const;
    
    template <class T>
    mat3<T>::mat3(const quat<T> &q)
    {
        m11 = -1.0+2.0*q.qx*q.qx+2.0*q.qw*q.qw;	m12 = 2.0*(q.qx*q.qy-q.qz*q.qw);		m13 = 2.0*(q.qx*q.qz+q.qy*q.qw);
        m21 = 2.0*(q.qx*q.qy+q.qz*q.qw);		m22 = -1.0+2.0*q.qy*q.qy+2.0*q.qw*q.qw;	m23 = 2.0*(q.qy*q.qz-q.qx*q.qw);
        m31 = 2.0*(q.qx*q.qz-q.qy*q.qw);		m32 = 2.0*(q.qx*q.qw+q.qy*q.qz);		m33 = -1.0+2.0*q.qz*q.qz+2.0*q.qw*q.qw;
    }
    // explicit template specialisation for <float>
    template mat3<float>::mat3(const quat<float> &q);
    
    template <class T>
    void mat3<T>::normalize()
    {
        vec3<T> r2 = vec3<T>(m12, m22, m23), r3 = vec3<T>(m13, m23, m33);
        r3.normalize();
        vec3<T> r1 = r2 % r3;
        r1.normalize();
        r2 = r3 % r1;
        m11 = r1.x; m12 = r2.x; m13 = r3.x;
        m21 = r1.y; m22 = r2.y; m23 = r3.y;
        m31 = r1.z; m32 = r2.z; m33 = r3.z;
    }
    // explicit template specialisation for <float>
    template void mat3<float>::normalize();
    
    template <class T>
    vec3<T> mat3<T>::operator*(const vec3<T> &v) const
    {
        return col[0]*v.x + col[1]*v.y + col[2]*v.z;
    }
    // explicit template specialisation for <float>
    template vec3<float> mat3<float>::operator*(const vec3<float> &v) const;
    
    template <class T>
    T mat3<T>::getEulerAngle()
    {
        return acos( (m11+m22+m33-1.0)*0.5 );
    }
    // explicit template specialisation for <float>
    template float mat3<float>::getEulerAngle();
    
    template <class T>
    vec3<T> mat3<T>::getEulerAxis(const T& theta)
    {
        T is = (T)1.0/(2.0*sin(theta));
        return vec3<T>(	(m32-m23)*is,
                       (m13-m31)*is,
                       (m21-m12)*is);
    }
    // explicit template specialisation for <float>
    template vec3<float> mat3<float>::getEulerAxis(const float& theta);
    
    template <class T>
    mat3<T> mat3<T>::getRdot(const vec3<T> &w)
    {
        return mat3<T>(-w.z*m21+w.y*m31, -w.z*m22+w.y*m32, -w.z*m23+w.y*m33,
                       w.z*m11-w.x*m31, w.z*m12-w.x*m32, w.z*m13-w.x*m33,
                       -w.y*m11+w.x*m21, -w.y*m12+w.x*m22, -w.y*m13+w.x*m23);
    }
    // explicit template specialisation for <float>
    template mat3<float> mat3<float>::getRdot(const vec3<float> &w);
    

    template <class T>
    mat4<T> mat4<T>::lookatRHS(const vec3<T>& eye, const vec3<T>& at, const vec3<T>& up)
    {
#if 1
        vec3<T> z = (eye - at).normalize();
        
        mat4<T> R = mat4<T>( mat3<T>::base(z, up) );
        R.transpose();
        
        return R * mat4<T>::translation(-eye);
#else
        vec3<T> z = (eye - at).normalize();
        vec3<T> x = (up % z).normalize();
        vec3<T> y = (z % x).normalize();
        
        mat3<T> R3 = mat3<T>(x, y, z);
        R3.transpose();
        
        mat4<T> R4 = mat4<T>(R3);
        mat4<T> M = R4 * mat4<T>::translation(-eye);
#endif
    }
    // explicit template specialisation for <float>
    template mat4<float> mat4<float>::lookatRHS(const vec3<float>& eye, const vec3<float>& at, const vec3<float>& up);
    
    
    template <class T>
    mat4<T> mat4<T>::lookatLHS(const vec3<T>& eye, const vec3<T>& at, const vec3<T>& up)
    {
        vec3<T> z = (at - eye).normalize();
        
        mat4<T> R = mat4<T>( mat3<T>::base(z, up) );
        R.transpose();
        
        return R * mat4<T>::translation(-eye);
    }
    // explicit template specialisation for <float>
    template mat4<float> mat4<float>::lookatLHS(const vec3<float>& eye, const vec3<float>& at, const vec3<float>& up);
    

    template <class T>
    mat4<T> mat4<T>::lookatRHS_inverse(const vec3<T> &eye, const vec3<T> &at, const vec3<T> &up)
    {
#if 1
        vec3<T> z = (eye - at).normalize();
        
        mat4<T> R = mat4<T>( mat3<T>::base(z, up) );
        
        return mat4<T>::translation(eye) * R;
#else
        vec3<T> z = (eye - at).normalize();
        vec3<T> x = (up % z).normalize();
        vec3<T> y = (z % x).normalize();
        
        mat3<T> R3 = mat3<T>(x, y, z);
        
        mat4<T> R4 = mat4<T>(R3);
        mat4<T> M = mat4<T>::translation(eye) * R4;
        
        return M;
#endif
    }
    // explicit template specialisation for <float>
    template mat4<float> mat4<float>::lookatRHS_inverse(const vec3<float> &eye, const vec3<float> &at, const vec3<float> &up);
    
    
    template <class T>
    mat4<T> mat4<T>::lookatLHS_inverse(const vec3<T> &eye, const vec3<T> &at, const vec3<T> &up)
    {
        vec3<T> z = (at - eye).normalize();
        
        mat4<T> R = mat4<T>( mat3<T>::base(z, up) );
        
        return mat4<T>::translation(eye) * R;
    }
    // explicit template specialisation for <float>
    template mat4<float> mat4<float>::lookatLHS_inverse(const vec3<float> &eye, const vec3<float> &at, const vec3<float> &up);
    
    
    template <class T>
    vec4<T> mat4<T>::operator *(const vec4<T> &v) const
    {
        return col[0]*v.x + col[1]*v.y + col[2]*v.z + col[3]*v.w;
    }
    // explicit template specialisation for <float>
    template vec4<float> mat4<float>::operator *(const vec4<float> &v) const;
    
    
    /*
     diagonalization of symmetrix 3x3 matrix
     used for inertia tensors
     
     borrowed directly from Bullet Physics Engine:
     
     https://github.com/bulletphysics/bullet3/blob/d347bca2bad80420869217282535fb4ff919e8b5/src/LinearMath/btMatrix3x3.h#L639
     
     used when computing inertia tensors for compound shapes
     
     https://github.com/bulletphysics/bullet3/blob/d347bca2bad80420869217282535fb4ff919e8b5/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp
     (old location:)
     https://code.google.com/p/bullet/source/browse/trunk/src/BulletCollision/CollisionShapes/btCompoundShape.cpp#209)
     */
    
    /**@brief diagonalizes this matrix by the Jacobi method.
     * @param rot stores the rotation from the coordinate system in which the matrix is diagonal to the original
     * coordinate system, i.e., old_this = rot * new_this * rot^T.
     * @param threshold See iteration
     * @param iteration The iteration stops when all off-diagonal elements are less than the threshold multiplied
     * by the sum of the absolute values of the diagonal, or when maxSteps have been executed.
     *
     * Note that this matrix is assumed to be symmetric.
     */
    template <class T>
    void mat3<T>::diagonalize(mat3<T> &rot, float threshold, int maxSteps)
    {
#define MAT3_DIAG_EPS 0.00001f
        //    rot.setIdentity();
        rot = mat3<T>(1,1,1);
        
        for (int step = maxSteps; step > 0; step--)
        {
            // find off-diagonal element [p][q] with largest magnitude
            int p = 0;
            int q = 1;
            int r = 2;
            float max = fabs(mat[0][1]);
            float v = fabs(mat[0][2]);
            if (v > max)
            {
                q = 2;
                r = 1;
                max = v;
            }
            v = fabs(mat[1][2]);
            if (v > max)
            {
                p = 1;
                q = 2;
                r = 0;
                max = v;
            }
            float t = threshold * (fabs(mat[0][0]) + fabs(mat[1][1]) + fabs(mat[2][2]));
            if (max <= t)
            {
                if (max <= MAT3_DIAG_EPS * t)
                {
                    return;
                }
                step = 1;
            }
            // compute Jacobi rotation J which leads to a zero for element [p][q]
            float mpq = mat[p][q];
            float theta = (mat[q][q] - mat[p][p]) / (2 * mpq);
            float theta2 = theta * theta;
            float cos;
            float sin;
            if (theta2 * theta2 < 10.0f/MAT3_DIAG_EPS)
            {
                t = (theta >= 0) ? 1.0f / (theta + sqrt(1.0f + theta2))
                : 1.0f / (theta - sqrt(1.0f + theta2));
                cos = 1.0f / sqrt(1.0f + t * t);
                sin = cos * t;
            }
            else
            {
                // approximation for large theta-value, i.e., a nearly diagonal matrix
                t = 1.0f / (theta * (2.0f + 0.5f / theta2));
                cos = 1.0f - 0.5f * t * t;
                sin = cos * t;
            }
            // apply rotation to matrix (this = J^T * this * J)
            mat[p][q] = mat[q][p] = 0;
            mat[p][p] -= t * mpq;
            mat[q][q] += t * mpq;
            float mrp = mat[r][p];
            float mrq = mat[r][q];
            mat[r][p] = mat[p][r] = cos * mrp - sin * mrq;
            mat[r][q] = mat[q][r] = cos * mrq + sin * mrp;
            // apply rotation to rot (rot = rot * J)
            for (int i = 0; i < 3; i++)
            {
                //            btVector3& row = rot[i];
                //            mrp = row[p];
                //            mrq = row[q];
                //            row[p] = cos * mrp - sin * mrq;
                //            row[q] = cos * mrq + sin * mrp;
                
                mrp = rot.mat[p][i];
                mrq = rot.mat[q][i];
                rot.mat[p][i] = cos * mrp - sin * mrq;
                rot.mat[q][i] = cos * mrq + sin * mrp;
            }
        }
    }
    // explicit template specialisation for <float>
    template void mat3<float>::diagonalize(mat3<float> &rot, float threshold, int maxSteps);
    
}
