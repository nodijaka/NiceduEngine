
/*
 * matrix lib
 *
 * Carl Johan Gribel (c) 2011-2016, cjgribel@gmail.com
 *
 * License: free to use but keep this section unchanged
 *
 */

#pragma once
#ifndef MAT3_H
#define MAT3_H

#include <cstdio>
#include "math_.h"
#include "vec.h"
#include "quat.h"

namespace linalg
{
    //
    // ------------------------ 2x2 column-major matrix ------------------------
    //
    // | m11 m12 |
    // | m21 m22 |
    //
    
    template<class T> class mat2
    {
    public:
        union {
            T array[4];
            T mat[2][2];
            struct { T m11, m21, m12, m22; };
            struct { vec2<T> col[2]; };
        };
        
        //
        // --------------------------- Constructors ----------------------------
        //
        
        mat2()
        {
            
        }
        
        // From row-first elements
        //
        mat2(const T& m11, const T& m12, const T& m21, const T& m22) : m11(m11), m12(m12), m21(m21), m22(m22)
        {
            this->m11 = m11; this->m12 = m12;
            this->m21 = m21; this->m22 = m22;
        }
        
        // From rotation matrix
        // Todo: this is a bit ambiguous
        //
        mat2(const T& rad)
        {
            T c = cos(rad);
            T s = sin(rad);
            m11 = c; m12 = -s;
            m21 = s; m22 = c;
        }
        
        // From diagonal elements
        //
        mat2(const T& scale_x, const T& scale_y)
        {
            m11 = scale_x;	m12 = 0.0;
            m21 = 0.0;		m22 = scale_y;
        }
        
        //
        // -------------------------- Matrix algebra ---------------------------
        //
        
        mat2<T> invert() const
        {
            T det = m11 * m22 - m12 * m21;
            
            return mat2<T>(m22, -m21, -m12, m11) * (1.0/det);
        }
        
        //
        // ------------------------ Operator overloads -------------------------
        //
        
        mat2<T> operator - ()
        {
            return mat2<T>(-m11, -m12, -m21, -m22);
        }
        
        mat2<T> operator * (const T& s) const
        {
            return mat2<T>(m11*s, m12*s, m21*s, m22*s);
        }
        
        vec2<T> operator * (const vec2<T> &rhs) const;
        
    };
    
    
    //
    // ------------------------ 3x3 column-major matrix ------------------------
    //
    
    template<class T> class mat3
    {
    public:
        
        union {
            T array[9];
            T mat[3][3];
            struct { T m11, m21, m31, m12, m22, m32, m13, m23, m33; };
            struct { vec3<T> col[3]; };
        };
        
        //
        // --------------------------- Constructors ----------------------------
        //
        
        mat3() { }
        
        // Row-major per-element constructor
        //
        mat3(const T& m11, const T& m12, const T& m13,
             const T& m21, const T& m22, const T& m23,
             const T& m31, const T& m32, const T& m33)
        {
            this->m11 = m11; this->m12 = m12; this->m13 = m13;
            this->m21 = m21; this->m22 = m22; this->m23 = m23;
            this->m31 = m31; this->m32 = m32; this->m33 = m33;
        }
        
        // From equal diagonal elements
        //
        mat3(const T& d) : mat3(d,d,d) { }
        
        // From diagonal elements
        //
        mat3(const T& d0, const T& d1, const T& d2)
        {
            m11 = d0;
            m22 = d1;
            m33 = d2;
            m12 = m13 = m21 = m23 = m31 = m32 = 0.0;
        }
        
        // From basis vectors -> matrix columns
        //
        mat3(const vec3<T>& e0, const vec3<T>& e1, const vec3<T>& e2)
        {
            col[0] = e0;
            col[1] = e1;
            col[2] = e2;
        }
        
        // From rotation quaternion
        //
        mat3(const quat<T> &q);
        
        vec3<T> column(int i)
        {
            assert(i<3);
            return col[i];
        }

        vec3<T> column(int i) const
        {
            assert(i<3);
            return col[i];
        }

        
        //
        // ---------------------------- Modifiers ------------------------------
        //
        
        void set(const mat3<T> &m)
        {
            col[0] = m.col[0];
            col[1] = m.col[1];
            col[2] = m.col[2];
        }
        
        //
        // ------------------------- Transformations --------------------------
        //
        
        //
        // Rotation by theta around unit vector u,
        // i.e. rotation from Euler angle & axis [1].
        // This is called Rodrigues rotation formula [2]
        //
        //             | 0  -z  y |
        // Ru(theta) = | z   0 -x | sin(theta) + (I - u.u^T)cos(theta) + u.u^T
        //             | -y  x  0 |
        //
        // Matrix form:
        // R = I + sin(theta)K + (1 - sin(theta))K^2
        // where K = [u] = skew-symmetric / cross product matrix of u
        //
        // [1] https://en.wikipedia.org/wiki/Rotation_matrix#Nested_dimensions
        // [2] https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
        //
        static mat3<T> rotation(const T& theta, const T& x, const T& y, const T& z)
        {
            mat3<T> R;
            T c1 = cos(theta);
            T c2 = 1.0-c1;
            T s = sin(theta);
            
            R.m11 = c1 + c2*x*x;	R.m12 = c2*x*y - s*z;	R.m13 = c2*x*z + s*y;
            R.m21 = c2*x*y + s*z;	R.m22 = c1 + c2*y*y;	R.m23 = c2*y*z - s*x;
            R.m31 = c2*x*z - s*y;	R.m32 = c2*y*z + s*x;	R.m33 = c1 + c2*z*z;
            
            return R;
        }
        
        //
        // Rotation from one unit vector f to another unit vector t
        // ----------------------------------------------------------------------
        // Provides a rotation matrix R(f,t) such that R(f,t)f = t
        // Uses Möller and Hughes's derivation without square roots or trigonometrics [1]
        // ----------------------------------------------------------------------
        // [1]  Efficiently Building a Matrix to Rotate One Vector to Another
        //      http://cs.brown.edu/~jfh/papers/Moller-EBA-1999/paper.pdf
        //
        // Other references:
        // [2]  Rodrigues formula exposed
        //      http://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
        // [3]  Rodrigues rotation formula on matrix form
        //      http://math.stackexchange.com/questions/293116/rotating-one-3d-vector-to-another
        
        //
        static mat3<T> rotation_to_vector(const vec3<T> &f, const vec3<T> &t)
        {
            vec3f v = f % t;
            float c = f.dot(t);
            mat3<T> R;
            
            // Handle case when vectors are nearly parallel
            if (1.0f-fabs(c) < 0.01f)
            {
                // Pick cardinal axis most orthogonal to f
                vec3f p;
                float xabs = fabs(f.x);
                float yabs = fabs(f.y);
                float zabs = fabs(f.z);
                if (xabs < yabs && xabs < zabs) p = {1,0,0};
                if (yabs < xabs && yabs < zabs) p = {0,1,0};
                if (zabs < xabs && zabs < yabs) p = {0,0,1};
                
                vec3f u = p-f;
                vec3f v = p-t;
                float udot = u.dot(u);
                float vdot = v.dot(v);
                float a = 2.0f/udot;
                float b = 2.0f/vdot;
                float c = 4.0f*u.dot(v)/(udot*vdot);

                for (int i=0; i<3; i++)
                    for (int j=0; j<3; j++)
                        R.mat[i][j] = a*u.vec[i]*u.vec[j] + b*v.vec[i]*v.vec[j] - c*v.vec[i]*u.vec[j];
                R = mat3<T>(1) - R;
            }
            
            // Normal case
            float h = (1.0f - c)/(v.dot(v));
            return mat3<T>( c + h*v.x*v.x,      h*v.x*v.y - v.z,    h*v.x*v.z + v.y,
                            h*v.x*v.y + v.z,    c + h*v.y*v.y,      h*v.y*v.z - v.x,
                            h*v.x*v.z - v.y,    h*v.y*v.z + v.x,    c + h*v.z*v.z);
        }
        
        //
        // Extract Euler angle & axis from rotation matrix
        //
        // http://en.wikipedia.org/wiki/Rotation_representation#Rotation_matrix_.E2.86.94_Euler_axis.2Fangle
        //
        T getEulerAngle();
        
        vec3<T> getEulerAxis(const T& theta);
        
        //
        // -------------------------- Matrix algebra ---------------------------
        //
        
        void transpose()
        {
            std::swap(m21, m12);
            std::swap(m31, m13);
            std::swap(m32, m23);
        }
        
        //
        // Inverse: A^(-1) = 1/det(A) * adjugate(A)
        //
        mat3<T> inverse() const
        {
            T det = determinant();
            assert(det > 1e-8);
            T idet = 1.0/det;
            
            mat3<T> M;
            M.m11 = (m22*m33 - m32*m23);
            M.m21 = -(m21*m33 - m31*m23);
            M.m31 = (m21*m32 - m31*m22);
            
            M.m12 = -(m12*m33 - m32*m13);
            M.m22 = (m11*m33 - m31*m13);
            M.m32 = -(m11*m32 - m31*m12);
            
            M.m13 = (m12*m23 - m22*m13);
            M.m23 = -(m11*m23 - m21*m13);
            M.m33 = (m11*m22 - m21*m12);
            
            return M*idet;
        }
        
        T determinant() const
        {
            return m11*m22*m33 + m12*m23*m31 + m13*m21*m32 - m11*m23*m32 - m12*m21*m33 - m13*m22*m31;
        }
        
        //
        // (Ugly )Matrix normalization
        //
        // Todo: better technique, e.g. Gram Schmidt
        //
        void normalize();
        
        //
        // Retrieve time-derivative matrix dR/dt for a
        // rotation matrix R and a given velocity vector w
        //
        //                                       | 0   -w.z  w.y|
        // dR/dt = [w]*R, where [w] = skew(w) =  | w.z  0   -w.x| (angular velocity tensor)
        //                                       |-w.y  w.x  0  |
        //
        // which is equivalent to skew(w) * R
        //
        // http://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions#Quaternion_.E2.86.94_angular_velocities
        //
        mat3<T> getRdot(const vec3<T> &w);
        
        void diagonalize(mat3<T> &rot, float threshold, int maxSteps);
        
        //
        // Skew-symmetric matrix a.k.a. cross product operator matrix
        //
        // [v] = skew(v)
        // v x u = [v] u
        //
        static mat3<T> skew(const vec3<T> &v)
        {
            return mat3<T>(0, -v.z, v.y,
                           v.z, 0, -v.x,
                           -v.y, v.x, 0);
        }
        
        //
        // Constructs an orthonormal basis matrix from a
        // forward vector z and a (candidate) up vector.
        // The up vector is replaced if too paralell to z.
        //
        static mat3<T> base(const vec3<T>& z, const vec3<T>& up_candidate = {0,1,0})
        {
            vec3<T> up = up_candidate;
            vec3<T> zn = linalg::normalize(z);
            
            // If up nearly parallell with z,
            // replace up with cardinal vector most orthogonal to z
            if ( 1.0f-fabs(up.dot(zn)) < 0.001f ) // 0.01
            {
                float xabs = fabs(zn.x);
                float yabs = fabs(zn.y);
                float zabs = fabs(zn.z);
                if (xabs < yabs && xabs < zabs)
                    up = {1,0,0};
                else if (yabs < xabs && yabs < zabs)
                    up = {0,1,0};
                else
                    up = {0,0,1};
            }
            vec3<T> xn = (up % zn).normalize();
            vec3<T> yn = (zn % xn).normalize();
            
            return { xn, yn, zn };
        }
        
        //
        // ------------------------- Operator overloads -----------------------
        //
        
        mat3<T> operator * (const T& s) const
        {
            return mat3<T>(m11*s, m12*s, m13*s,
                           m21*s, m22*s, m23*s,
                           m31*s, m32*s, m33*s);
        }
        
        mat3<T> operator +(const mat3<T>& m) const
        {
            return mat3<T>(m11+m.m11, m12+m.m12, m13+m.m13,
                           m21+m.m21, m22+m.m22, m23+m.m23,
                           m31+m.m31, m32+m.m32, m33+m.m33);
        }
        
        mat3<T> operator -(const mat3<T>& m) const
        {
            return mat3(m11-m.m11, m12-m.m12, m13-m.m13,
                        m21-m.m21, m22-m.m22, m23-m.m23,
                        m31-m.m31, m32-m.m32, m33-m.m33);
        }
        
        mat3<T>& operator +=(const mat3<T>& m)
        {
            col[0] += m.col[0];
            col[1] += m.col[1];
            col[2] += m.col[2];
            
            return *this;
        }
        
        mat3<T>& operator *=(const T& s)
        {
            col[0] *= s;
            col[1] *= s;
            col[2] *= s;
            
            return *this;
        }
        
        mat3<T> operator *(const mat3<T>& m) const
        {
            return mat3<T>(m11*m.m11+m12*m.m21+m13*m.m31, m11*m.m12+m12*m.m22+m13*m.m32, m11*m.m13+m12*m.m23+m13*m.m33,
                           m21*m.m11+m22*m.m21+m23*m.m31, m21*m.m12+m22*m.m22+m23*m.m32, m21*m.m13+m22*m.m23+m23*m.m33,
                           m31*m.m11+m32*m.m21+m33*m.m31, m31*m.m12+m32*m.m22+m33*m.m32, m31*m.m13+m32*m.m23+m33*m.m33);
        }
        
        vec3<T> operator *(const vec3<T> &v) const;
        
        //
        // ----------------------------- Auxiliary -----------------------------
        //
        
        // Thread safe print
        //
        void debugPrint()
        {
            printf("%1.5f %1.5f %1.5f\n%1.5f %1.5f %1.5f\n%1.5f %1.5f %1.5f\n",
                   m11, m12, m13, m21, m22, m23, m31, m32, m33);
            //		printf("[%1.2f %1.2f %1.2f; %1.2f %1.2f %1.2f; %1.2f %1.2f %1.2f]\n\n",
            //               m11, m12, m13, m21, m22, m23, m31, m32, m33);
        }
        
    };
    
    // Thread unsafe print
    //
    template<class T>
    inline std::ostream& operator<< (std::ostream &out, const mat3<T> &m)
    {
        for (int i=0; i<3; i++)
            printf("%1.4f, %1.4f, %1.4ff\n", m.mat[0][i], m.mat[1][i], m.mat[2][i]);
        
        return out;
    }
    
    //
    // ------------------------ 4x4 column-major matrix ------------------------
    //
    template<class T> class mat4
    {
    public:
        
        union
        {
            T array[16];
            T mat[4][4];
            struct
            {
                T   m11, m21, m31, m41,
                    m12, m22, m32, m42,
                    m13, m23, m33, m43,
                    m14, m24, m34, m44;
            };
            struct
            {
                vec4<T> col[4];
            };
        };
        
        //
        // --------------------------- Constructors ----------------------------
        //
        
        // Trivial
        //
        mat4() { }
        
        // From equal diagonal elements
        //
        mat4(T d) : mat4(d,d,d,d) { }
        
        // From diagonal elements
        //
        mat4(const T& d0, const T& d1, const T& d2, const T& d3)
        {
            m11 = d0;  m12 = 0.0; m13 = 0.0; m14 = 0.0;
            m21 = 0.0; m22 = d1;  m23 = 0.0; m24 = 0.0;
            m31 = 0.0; m32 = 0.0; m33 = d2;  m34 = 0.0;
            m41 = 0.0; m42 = 0.0; m43 = 0.0; m44 = d3;
        }
        
        // From 3x3-matrix
        //
        mat4(const mat3<T> &m)
        {
            m11 = m.m11; m12 = m.m12; m13 = m.m13; m14 = 0.0;
            m21 = m.m21; m22 = m.m22; m23 = m.m23; m24 = 0.0;
            m31 = m.m31; m32 = m.m32; m33 = m.m33; m34 = 0.0;
            m41 = 0.0;   m42 = 0.0;   m43 = 0.0;   m44 = 1.0;
        }
        
        // Row-major per-element constructor
        //
        mat4(const T& m11, const T& m12, const T& m13, const T& m14,
             const T& m21, const T& m22, const T& m23, const T& m24,
             const T& m31, const T& m32, const T& m33, const T& m34,
             const T& m41, const T& m42, const T& m43, const T& m44)
        {
            this->m11 = m11; this->m12 = m12; this->m13 = m13; this->m14 = m14;
            this->m21 = m21; this->m22 = m22; this->m23 = m23; this->m24 = m24;
            this->m31 = m31; this->m32 = m32; this->m33 = m33; this->m34 = m34;
            this->m41 = m41; this->m42 = m42; this->m43 = m43; this->m44 = m44;
        }
        
        // From basis vectors -> matrix columns
        //
        mat4(const vec4<T>& e0, const vec4<T>& e1, const vec4<T>& e2, const vec4<T>& e3)
        {
            col[0] = e0;
            col[1] = e1;
            col[2] = e2;
            col[3] = e3;
        }
        
        //
        // ---------------------------- Modifiers ------------------------------
        //
        
        void set(const mat4<T> &m)
        {
            col[0] = m.col[0];
            col[1] = m.col[1];
            col[2] = m.col[2];
            col[3] = m.col[3];
        }
        
        //    vec4 row(int i)
        //    {
        //        assert(j<4);
        //
        //        return vec4(mat[0][i], mat[1][i], mat[2][i], mat[3][i]);
        //    }
        
        // todo: return vec4<T>&
        vec4<T> column(int i)
        {
            assert(i<4);
            return col[i];
        }
        
        vec4<T> column(int i) const
        {
            assert(i<4);
            return col[i];
        }
        
        T& operator [](unsigned i) const
        {
            return array[i];
        }
        
        T& operator [](unsigned i)
        {
            return array[i];
        }
        
        mat3<T> get_3x3() const
        {
            return mat3<T>(m11, m12, m13, m21, m22, m23, m31, m32, m33);
        }
        
        //
        // -------------------------- Matrix algebra ---------------------------
        //
        
        void transpose()
        {
            std::swap(m21, m12);
            std::swap(m31, m13);
            std::swap(m32, m23);
            std::swap(m41, m14);
            std::swap(m42, m24);
            std::swap(m43, m34);
        }
        
        mat4<T> inverse() const
        {
            T det = determinant();
            //assert(abs(det) > 1e-8);
            T idet = 1.0/det;
            
            mat4<T> M = mat4<T>(m23 * m34 * m42 - m24 * m33 * m42 + m24 * m32 * m43 - m22 * m34 * m43 - m23 * m32 * m44 + m22 * m33 * m44,
                                m14 * m33 * m42 - m13 * m34 * m42 - m14 * m32 * m43 + m12 * m34 * m43 + m13 * m32 * m44 - m12 * m33 * m44,
                                m13 * m24 * m42 - m14 * m23 * m42 + m14 * m22 * m43 - m12 * m24 * m43 - m13 * m22 * m44 + m12 * m23 * m44,
                                m14 * m23 * m32 - m13 * m24 * m32 - m14 * m22 * m33 + m12 * m24 * m33 + m13 * m22 * m34 - m12 * m23 * m34,
                                m24 * m33 * m41 - m23 * m34 * m41 - m24 * m31 * m43 + m21 * m34 * m43 + m23 * m31 * m44 - m21 * m33 * m44,
                                m13 * m34 * m41 - m14 * m33 * m41 + m14 * m31 * m43 - m11 * m34 * m43 - m13 * m31 * m44 + m11 * m33 * m44,
                                m14 * m23 * m41 - m13 * m24 * m41 - m14 * m21 * m43 + m11 * m24 * m43 + m13 * m21 * m44 - m11 * m23 * m44,
                                m13 * m24 * m31 - m14 * m23 * m31 + m14 * m21 * m33 - m11 * m24 * m33 - m13 * m21 * m34 + m11 * m23 * m34,
                                m22 * m34 * m41 - m24 * m32 * m41 + m24 * m31 * m42 - m21 * m34 * m42 - m22 * m31 * m44 + m21 * m32 * m44,
                                m14 * m32 * m41 - m12 * m34 * m41 - m14 * m31 * m42 + m11 * m34 * m42 + m12 * m31 * m44 - m11 * m32 * m44,
                                m12 * m24 * m41 - m14 * m22 * m41 + m14 * m21 * m42 - m11 * m24 * m42 - m12 * m21 * m44 + m11 * m22 * m44,
                                m14 * m22 * m31 - m12 * m24 * m31 - m14 * m21 * m32 + m11 * m24 * m32 + m12 * m21 * m34 - m11 * m22 * m34,
                                m23 * m32 * m41 - m22 * m33 * m41 - m23 * m31 * m42 + m21 * m33 * m42 + m22 * m31 * m43 - m21 * m32 * m43,
                                m12 * m33 * m41 - m13 * m32 * m41 + m13 * m31 * m42 - m11 * m33 * m42 - m12 * m31 * m43 + m11 * m32 * m43,
                                m13 * m22 * m41 - m12 * m23 * m41 - m13 * m21 * m42 + m11 * m23 * m42 + m12 * m21 * m43 - m11 * m22 * m43,
                                m12 * m23 * m31 - m13 * m22 * m31 + m13 * m21 * m32 - m11 * m23 * m32 - m12 * m21 * m33 + m11 * m22 * m33);
            
            return M*idet;
        }
        
        T determinant() const
        {
            return
            m14 * m23 * m32 * m41 - m13 * m24 * m32 * m41 - m14 * m22 * m33 * m41 + m12 * m24 * m33 * m41 +
            m13 * m22 * m34 * m41 - m12 * m23 * m34 * m41 - m14 * m23 * m31 * m42 + m13 * m24 * m31 * m42 +
            m14 * m21 * m33 * m42 - m11 * m24 * m33 * m42 - m13 * m21 * m34 * m42 + m11 * m23 * m34 * m42 +
            m14 * m22 * m31 * m43 - m12 * m24 * m31 * m43 - m14 * m21 * m32 * m43 + m11 * m24 * m32 * m43 +
            m12 * m21 * m34 * m43 - m11 * m22 * m34 * m43 - m13 * m22 * m31 * m44 + m12 * m23 * m31 * m44 +
            m13 * m21 * m32 * m44 - m11 * m23 * m32 * m44 - m12 * m21 * m33 * m44 + m11 * m22 * m33 * m44;
        }
        
        //
        // ------------------------- Operator overloads -----------------------
        //
        
        mat4<T> operator *(const T& s) const
        {
            return mat4<T>(m11*s, m12*s, m13*s, m14*s,
                           m21*s, m22*s, m23*s, m24*s,
                           m31*s, m32*s, m33*s, m34*s,
                           m41*s, m42*s, m43*s, m44*s);
        }
        
        mat4<T>& operator *=(const T& s)
        {
            col[0] *= s;
            col[1] *= s;
            col[2] *= s;
            col[3] *= s;
            
            return *this;
        }
        
        mat4<T> operator + (const mat4<T>& m) const
        {
            mat4<T> n = *this;
            n.col[0] += m.col[0];
            n.col[1] += m.col[1];
            n.col[2] += m.col[2];
            n.col[3] += m.col[3];
            
            return n;
        }
        
        mat4<T> operator *(const mat4<T>& m) const
        {
            return mat4<T>(m11 * m.m11 + m12 * m.m21 + m13 * m.m31 + m14 * m.m41,
                           m11 * m.m12 + m12 * m.m22 + m13 * m.m32 + m14 * m.m42,
                           m11 * m.m13 + m12 * m.m23 + m13 * m.m33 + m14 * m.m43,
                           m11 * m.m14 + m12 * m.m24 + m13 * m.m34 + m14 * m.m44,
                           
                           m21 * m.m11 + m22 * m.m21 + m23 * m.m31 + m24 * m.m41,
                           m21 * m.m12 + m22 * m.m22 + m23 * m.m32 + m24 * m.m42,
                           m21 * m.m13 + m22 * m.m23 + m23 * m.m33 + m24 * m.m43,
                           m21 * m.m14 + m22 * m.m24 + m23 * m.m34 + m24 * m.m44,
                           
                           m31 * m.m11 + m32 * m.m21 + m33 * m.m31 + m34 * m.m41,
                           m31 * m.m12 + m32 * m.m22 + m33 * m.m32 + m34 * m.m42,
                           m31 * m.m13 + m32 * m.m23 + m33 * m.m33 + m34 * m.m43,
                           m31 * m.m14 + m32 * m.m24 + m33 * m.m34 + m34 * m.m44,
                           
                           m41 * m.m11 + m42 * m.m21 + m43 * m.m31 + m44 * m.m41,
                           m41 * m.m12 + m42 * m.m22 + m43 * m.m32 + m44 * m.m42,
                           m41 * m.m13 + m42 * m.m23 + m43 * m.m33 + m44 * m.m43,
                           m41 * m.m14 + m42 * m.m24 + m43 * m.m34 + m44 * m.m44);
        }
        
        vec4<T> operator *(const vec4<T> &v) const;
        
        //
        // ------------------------- Transformations --------------------------
        //
        
        static mat4<T> translation(const vec3<T>& p)
        {
            return translation(p.x, p.y, p.z);
        }
        
        static mat4<T> translation(const T& x, const T& y, const T& z)
        {
            mat4<T> M;
            M.m11 = 1.0; M.m12 = 0.0; M.m13 = 0; M.m14 = x;
            M.m21 = 0.0; M.m22 = 1.0; M.m23 = 0; M.m24 = y;
            M.m31 = 0.0; M.m32 = 0.0; M.m33 = 1; M.m34 = z;
            M.m41 = 0.0; M.m42 = 0.0; M.m43 = 0; M.m44 = 1.0;
            
            return M;
        }
        
        static mat4<T> scaling(const vec3<T> &sv)
        {
            return mat4<T>(sv.x, sv.y, sv.z, 1.0);
        }
        
        static mat4<T> scaling(const T& s)
        {
            return scaling({s,s,s});
        }
        
        static mat4<T> scaling(float sx, float sy, float sz)
        {
            return mat4<T>(sx, sy, sz, 1.0);
        }
        
        //
        // Rotation by theta around unit vector u,
        // i.e. rotation from Euler angle & axis [1].
        // This is called Rodrigues rotation formula [2]
        //
        //             | 0  -z  y |
        // Ru(theta) = | z   0 -x | sin(theta) + (I - u.u^T)cos(theta) + u.u^T
        //             | -y  x  0 |
        //
        // Matrix form:
        // R = I + sin(theta)K + (1 - sin(theta))K^2
        // where K = [u] = skew-symmetric / cross product matrix of u
        //
        // [1] https://en.wikipedia.org/wiki/Rotation_matrix#Nested_dimensions
        // [2] https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
        //
        static mat4<T> rotation(const T& theta, const T& x, const T& y, const T& z)
        {
//            mat4<T> M;
//            T c1 = cos(theta);
//            T c2 = 1.0-c1;
//            T s = sin(theta);
//            
//            M.m11 = c1 + c2*x*x;	M.m12 = c2*x*y - s*z;	M.m13 = c2*x*z + s*y;   M.m14 = 0.0;
//            M.m21 = c2*x*y + s*z;	M.m22 = c1 + c2*y*y;	M.m23 = c2*y*z - s*x;   M.m24 = 0.0;
//            M.m31 = c2*x*z - s*y;	M.m32 = c2*y*z + s*x;	M.m33 = c1 + c2*z*z;;   M.m34 = 0.0;
//            M.m41 = 0.0;            M.m42 = 0.0;            M.m43 = 0.0;            M.m44 = 1.0;
            
            return mat4<T>( mat3<T>::rotation(theta, x, y, z) );
            //return M;
        }
        
        static mat4<T> rotation(const T& theta, const vec3<T> &v)
        {
            return rotation(theta, v.x, v.y, v.z);
        }
        
        //
        // Rotation from one vector to another
        // Provides a rotation matrix which aligns vector u with vector v
        //
        // http://math.stackexchange.com/questions/293116/rotating-one-3d-vector-to-another
        // http://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
        //
        static mat4<T> rotation(const vec3<T> &u, const vec3<T> &v)
        {
            return mat4<T>( mat3<T>::rotaton(u, v) );
        }
        
        //
        // Rotation from Euler angles: roll (z), yaw (y) , pitch (x)
        // Mult order, R = R_z(roll) * R_y(yaw) * R_x(pitch)
        //
        // Matrix definition:
        // http://planning.cs.uiuc.edu/node102.html
        // Note: uses notation yaw (z), roll (x), pitch (y)
        //
        // (alternative: DirextX::Matrix.RotationYawPitchRoll, same notation)
        // https://msdn.microsoft.com/en-us/library/windows/desktop/bb281744(v=vs.85).aspx
        // Note: order R = R_y(yaw) * R_x(pitch) * R_z(roll)
        //
        static mat4<T> rotation(const T& roll, const T& yaw, const T& pitch)
        {
            const T sina = sin(roll);
            const T cosa = cos(roll);
            const T sinb = sin(yaw);
            const T cosb = cos(yaw);
            const T sing = sin(pitch);
            const T cosg = cos(pitch);
            
            return mat4<T>(cosa*cosb, cosa*sinb*sing-sina*cosg,  cosa*sinb*cosg-sina*sing, 0,
                           sina*cosb, sina*sinb*sing+cosa*cosg,  sina*sinb*cosg-cosa*sing, 0,
                           -sinb, cosb*sing, cosb*cosg, 0,
                           0, 0, 0, 1);
        }
        
        // TRS composite transform
        //
        static mat4<T> TRS(vec3<T> vt, float theta, vec3<T> rotv, vec3<T> sv)
        {
            return translation(vt) * rotation(theta, rotv) * scaling(sv);
        }
        
        //
        // -------------------- Look-At view transformations -------------------
        //
        
        //
        // Right-handed LookAt view matrix (view direction along -z)
        //
        // Transforms World -> View space (inverse of camera transform)
        //
        static mat4<T> lookatRHS(const vec3<T> &eye, const vec3<T> &at, const vec3<T> &up);
        
        //
        // Left-handed LookAt view matrix (view direction along +z)
        //
        // Transforms World -> View space (inverse of camera transform)
        //
        static mat4<T> lookatLHS(const vec3<T> &eye, const vec3<T> &at, const vec3<T> &up);
        
        //
        // Inverse of right-handed LookAt view matrix (view direction along -z)
        //
        // Transforms View -> World space (camera transform)
        //
        static mat4<T> lookatRHS_inverse(const vec3<T> &eye, const vec3<T> &at, const vec3<T> &up);
        
        //
        // Inverse of left-handed LookAt view matrix (view direction along +z)
        //
        // Transforms View -> World space (camera transform)
        //
        static mat4<T> lookatLHS_inverse(const vec3<T> &eye, const vec3<T> &at, const vec3<T> &up);
        
        // --------------- OpenGL perspective projection matrices --------------
        //
        // Symmetric/centred and asymmetric/off-center versions
        // for right-handed systems.
        //
        // In DirextX, z in NDC ranges [0,1] whereas in GL z ranges [-1,1]
        //
        // Reference: [18]

        static mat4<T> GL_AsymmetricPerspectiveProjectionRHS(const T& l, const T& r, const T& b, const T& t, const T& n, const T& f)
        {
            T n2 = 2.0f*n;
            T rl = r - l;
            T tb = t - b;
            T fn = f - n;
            
            return mat4<T>(n2/rl,   0.0f,   (r+l)/rl,   0.0f,
                           0.0f,    n2/tb,  (t+b)/tb,   0.0f,
                           0.0f,    0.0f,   (-f- n)/fn, (-n2*f)/fn,
                           0.0f,    0.0f,   -1.0f,      0.0f);
        }
        
        static mat4<T> GL_SymmetricPerspectiveProjectionRHS(const T& r, const T& t, const T& n, const T& f)
        {
            T n2 = 2.0f*n;
            T fn = f - n;
            
            return mat4<T>(n/r,   0.0f, 0.0f,       0.0f,
                           0.0f,  n/t,  0.0f,       0.0f,
                           0.0f,  0.0f, (-f-n)/fn,  (-n2*f)/fn,
                           0.0f,  0.0f, -1.0f,      0.0f);
        }
        
        //
        // GL default projection matrix [18]
        //
        // vfov in radians
        //
        static mat4<T> GL_PerspectiveProjectionRHS(const T& vfov, const T& ar, const T& n, const T& f)
        {
            T t = n * tanf(vfov/2.0f);
            T r = t * ar;
            
            return GL_SymmetricPerspectiveProjectionRHS(r, t, n, f);
        }
        
        // -------------- OpenGL orthographic projection matrices --------------
        //
        // Symmetric/centred and asymmetric/off-center versions
        // for right-handed systems.
        //
        // In DirextX, z in NDC ranges [0,1] whereas in GL z ranges [-1,1]
        //
        // Reference: [18]
        // ---------------------------------------------------------------------
        
        
        static mat4<T> GL_AsymmetricOrthoProjectionRHS(const T& l, const T& r, const T& b, const T& t, const T& n, const T& f)
        {
            return mat4<T>(2.0f/(r-l),  0.0f,       0.0f,       (r+l)/(l-r),
                           0.0f,        2.0f/(t-b), 0.0f,       (b+t)/(b-t),
                           0.0f,        0.0f,       2.0f/(n-f), (f+n)/(n-f),
                           0.0f,        0.0f,       0.0f,       1.0f);
        }
        
        static mat4<T> GL_SymmetricOrthoProjectionRHS(const T& r, const T& t, const T& n, const T& f)
        {
            return mat4<T>(1.0f/r,  0.0f,   0.0f,       0.0f,
                           0.0f,    1.0f/t, 0.0f,       0.0f,
                           0.0f,    0.0f,   -2.0f/(f-n), -(f+n)/(f-n),
                           0.0f,    0.0f,   0.0f,       1.0f);
        }
        
        static mat4<T> GL_OrthoProjectionRHS(const T& r, const T& t, const T& n, const T& f)
        {
            return GL_SymmetricOrthoProjectionRHS(r, t, n, f);
        }
        
        //
        // Returns the inverse of a GL perspective projection matrix
        // Should work for symmetric and off-center versions, as well as LH/RH.
        //
        // Derivation: https://docs.google.com/document/d/12zwtHHG7sj5aCY47fW2IPkye1XDoUWcnIvansodr7u8/edit#heading=h.smi3mje0dw6t
        //
        // Todo #1: DirectX versions can be derived from this as well.
        //
        // Todo #2: A version for orthographic projection (which has a
        //          different form compared to perspective projection) could
        //          borrow the Viewport inverse, since these transformations are
        //          structurally the same.
        //
        static mat4<T> GL_PerspectiveProjectionInverse(const mat4<T>& p)
        {
            return mat4<T>(1.0f/p.m11,  0.0f,       0.0f,       -p.m13/p.m11/p.m43,
                           0.0f,        1.0f/p.m22, 0.0f,       -p.m23/p.m22/p.m43,
                           0.0f,        0.0f,       0.0f,       1.0f/p.m43,
                           0.0f,        0.0f,       1.0f/p.m34, -p.m33/p.m34/p.m43);
        }
        
        
        // ------------- DirectX orthographic projection matrices --------------
        //
        // Symmetric/centred and asymmetric/off-center versions,
        // for right-handed and left-handed systems.
        //
        // In DirextX, z in NDC ranges [0,1] whereas in GL z ranges [-1,1]
        // ---------------------------------------------------------------------
        
        
        // D3DXMatrixOrthoOffCenterRH: https://msdn.microsoft.com/en-us/library/windows/desktop/bb205348(v=vs.85).aspx
        //
        static mat4<T> DX_AsymmetricOrthoProjectionRHS(const T& l, const T& r, const T& b, const T& t, const T& n, const T& f)
        {
            return mat4<T>(2.0f/(r-l),  0.0f,       0.0f,       (r+l)/(l-r),
                           0.0f,        2.0f/(t-b), 0.0f,       (b+t)/(b-t),
                           0.0f,        0.0f,       1.0f/(n-f), n/(n-f),
                           0.0f,        0.0f,       0.0f,       1.0f);
        }
        
        // D3DXMatrixOrthoOffCenterLH: https://msdn.microsoft.com/en-us/library/windows/desktop/bb205347(v=vs.85).aspx
        //
        static mat4<T> DX_AsymmetricOrthoProjectionLHS(const T& l, const T& r, const T& b, const T& t, const T& n, const T& f)
        {
            return mat4<T>(2.0f/(r-l),  0.0f,       0.0f,       (r+l)/(l-r),
                           0.0f,        2.0f/(t-b), 0.0f,       (b+t)/(b-t),
                           0.0f,        0.0f,       1.0f/(f-n), n/(n-f),
                           0.0f,        0.0f,       0.0f,       1.0f);
        }
        
        // D3DXMatrixOrthoRH: https://msdn.microsoft.com/en-us/library/windows/desktop/bb205349(v=vs.85).aspx
        //
        static mat4<T> DX_SymmetricOrthoProjectionRHS(const T& w, const T& h, const T& n, const T& f)
        {
            return mat4<T>(2.0f/w,  0.0f,   0.0f,       0.0f,
                           0.0f,    2.0f/h, 0.0f,       0.0f,
                           0.0f,    0.0f,   1.0f/(n-f), n/(n-f),
                           0.0f,    0.0f,   0.0f,       1.0f);
        }
        
        // D3DXMatrixOrthoLH: https://msdn.microsoft.com/en-us/library/windows/desktop/bb205346(v=vs.85).aspx
        //
        static mat4<T> DX_SymmetricOrthoProjectionLHS(const T& w, const T& h, const T& n, const T& f)
        {
            return mat4<T>(2.0f/w,  0.0f,   0.0f,       0.0f,
                           0.0f,    2.0f/h, 0.0f,       0.0f,
                           0.0f,    0.0f,   1.0f/(f-n), -n/(f-n),
                           0.0f,    0.0f,   0.0f,       1.0f);
        }
        
        static mat4<T> DX_OrthoProjectionRHS(const T& r, const T& t, const T& n, const T& f)
        {
            return DX_SymmetricOrthoProjectionRHS(r, t, n, f);
        }
        
        static mat4<T> DX_OrthoProjectionLHS(const T& r, const T& t, const T& n, const T& f)
        {
            return DX_SymmetricOrthoProjectionLHS(r, t, n, f);
        }
        
        // ------------------ OpenGL Viewport transformation -------------------
        
        // General viewport transform with support for sub viewports
        //
        static mat4<T> GL_Viewport(float l, float r, float t, float b, float n, float f)
        {
            return mat4<T>((r-l)/2,     0,          0,          (r+l)/2,
                           0,           (t-b)/2,    0,          (t+b)/2,
                           0,           0,          (f-n)/2,    (f+n)/2,
                           0,           0,          0,          1);
        }
        
        // Viewport inverse
        //
        // Derivation: https://docs.google.com/document/d/12zwtHHG7sj5aCY47fW2IPkye1XDoUWcnIvansodr7u8/edit#heading=h.kkf4cv27evh
        //
        static mat4<T> GL_ViewportInverse(const mat4<T>& vp)
        {
            return mat4<T>(1.0f/vp.m11,    0,              0,              -vp.m14/vp.m11,
                           0,              1.0f/vp.m22,    0,              -vp.m24/vp.m22,
                           0,              0,              1.0f/vp.m33,    -vp.m34/vp.m33,
                           0,              0,              0,              1.0f);
        }
        
#if 0
        //
        // Frustum plane from the projection matrix [19 + paper http://www8.cs.umu.se/kurser/5DV051/HT12/lab/plane_extraction.pdf]
        //
        // Six planes: pi = p[4] +/- p[i], i=1,2,3
        // The resulting plane/vec4 plane pi is the plane/half-space pi.(x,y,z,1) >= 0 where (x,y,z,1) is in View space
        //
        // P = Projection matrix -> plane in View space
        // P = P * WV -> plane in World space
        // etc
        //
        enum FRUSTUM_PLANE { LEFT, RIGHT, BOTTOM, UP, NEAR, FAR };
        
        static vec4<T> getFrustumPlanFromTransformation(const mat4<T>& P, FRUSTUM_PLANE plane)
        {
            vec4<T> p;
            switch (plane)
            {
                case LEFT:      p = row(3)+row(0); break;
                case RIGHT:     p = row(3)-row(0); break;
                case BOTTOM:    p = row(3)+row(1); break;
                case TOP:       p = row(3)-row(1); break;
                case NEAR:      p = row(3)+row(2); break;
                case FAR:       p = row(3)-row(2); break;
            }
            return p;
        }
#endif
        
        //
        // --------------------------- Miscellaneous ---------------------------
        //
        
        //
        // Shadow Projection: Matrix for projecting on a plane wrt a light source
        //
        // Form:
        // https://msdn.microsoft.com/en-us/library/windows/desktop/bb205364%28v=vs.85%29.aspx
        // Derivation (slide 9-):
        // http://web.cse.ohio-state.edu/~whmin/courses/cse5542-2013-spring/19-shadow.pdf
        //
        mat4<T> shadowProjection(const vec3<T> lightP, const vec3<T> planeP, const vec3<T> planeN)
        {
//            P = normalize(Plane);
//            L = Light;
//            d = -dot(P, L)
//            
//            P.a * L.x + d  P.a * L.y      P.a * L.z      P.a * L.w
//            P.b * L.x      P.b * L.y + d  P.b * L.z      P.b * L.w
//            P.c * L.x      P.c * L.y      P.c * L.z + d  P.c * L.w
//            P.d * L.x      P.d * L.y      P.d * L.z      P.d * L.w + d
        }
        
        
//        void write_to_file(std::string filename)
//        {
//            //
//        }
//        
//        void load_from_file(std::string filename)
//        {
//            //
//        }
        
        // Thread safe print
        //
        void debugPrint()
        {
            printf("%1.4f %1.4f %1.4f %1.4f\n%1.4f %1.4f %1.4f %1.4f\n%1.4f %1.4f %1.4f %1.4f\n%1.4f %1.4f %1.4f %1.4f\n",
                   m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
        }
        
    };
    
    // Thread unsafe print
    //
    template<class T>
    inline std::ostream& operator<< (std::ostream &out, const mat4<T> &m)
    {
        for (int i=0; i<4; i++)
            printf("%f, %f, %f, %f\n", m.mat[0][i], m.mat[1][i], m.mat[2][i], m.mat[3][i]);
//            printf("%1.4f, %1.4f, %1.4f, %1.4f\n", m.mat[0][i], m.mat[1][i], m.mat[2][i], m.mat[3][i]);
        
        return out;
    }

    
    template<class T>
    inline mat3<T> transpose(const mat3<T>& m)
    {
        mat3<T> n = m;
        n.transpose();
        return n;
    }
    
    template<class T>
    inline mat4<T> transpose(const mat4<T>& m)
    {
        mat4<T> n = m;
        n.transpose();
        return n;
    }
    
    template<class T>
    inline vec3<T> extract_translation(const mat4<T>& m)
    {
        return { m.m14, m.m24, m.m34 };
    }
    
    typedef mat2<float> mat2f;
    typedef mat3<float> mat3f;
    typedef mat4<float> mat4f;
    
    typedef mat2<float> m2f;
    typedef mat3<float> m3f;
    typedef mat4<float> m4f;
    
    //
    // Compile-time instances
    //
    const mat2f mat2f_zero = mat2f(0);
    const mat3f mat3f_zero = mat3f(0);
    const mat4f mat4f_zero = mat4f(0);
    const mat2f mat2f_identity = mat2f(1);
    const mat3f mat3f_identity = mat3f(1);
    const mat4f mat4f_identity = mat4f(1);
    
    const mat2f m2f_0 = mat2f(0);
    const mat3f m3f_0 = mat3f(0);
    const mat4f m4f_0 = mat4f(0);
    const mat2f m2f_1 = mat2f(1);
    const mat3f m3f_1 = mat3f(1);
    const mat4f m4f_1 = mat4f(1);
    
    /*
     Operations for general M x N matrix:
     Transpose (spawn N x M matrix)
     determinant, rank, trace, value_space, null_space
     Decompositions; LU, Cholesky, Helmholtz
     Solvers: Jacobi, Gauss-Seidel (solve 'this' matrix)
     Eigenvalues, Eigenvectors, Jordan normal form
     Inverse, Diagonalize
     
     Special op for square matrix:
     
     */
    
    
    /*
     todo
     general matrix
     not templated dimensions: dimensions not always known in compile time
     
     */
    
//    template<int >
    
    class mx
    {
    public:
        unsigned M, N;
        float *elem;
        
        mx(const unsigned &M, const unsigned &N)
        {
            assert(M>0 && N>0);
            
            this->M = M;
            this->N = N;
            elem = new float[N*M];
            
            for(int m=0; m<M; m++)
                for(int n=0; n<N; n++)
                {
                    elem[n*M+m] = (n==m ? 1 : 0);
                }
        }
        
        void set_all(const float &x)
        {
            for(int m=0; m<M; m++)
                for(int n=0; n<N; n++)
                {
                    elem[n*M+m] = x;
                }
        }
        
        void set_rand()
        {
            for(int m=0; m<M; m++)
                for(int n=0; n<N; n++)
                {
                    elem[n*M+m] = (int)(rnd(0, 1)*9);
                }
        }
        
        float& operator() (const int &m, const int &n) const
        {
            assert(m < M);
            assert(n < N);
            
            return elem[n*M+m];
        }
        
        mx operator* (const mx &rhs) const
        {
            assert(N == rhs.M);
            
            mx res = mx(M, rhs.N);
            // ...
            
            return res;
        }
        
        mx read_block(const unsigned &m_start, const unsigned &m_end, const unsigned &n_start, const unsigned &n_end)
        {
            assert(m_end >= m_start && m_end <= M);
            assert(n_end >= n_start && n_end <= N);
            
            mx a = mx(m_end-m_start+1, n_end-n_start+1);
            
            for(int m = 0; m < a.M; m++)
                for(int n = 0; n < a.N; n++)
                {
                    a(m, n) = (*this)(m_start + m, n_start + n);
                }
            
            return a;
        }
        
        void write_block(const unsigned &m_start, const unsigned &n_start, const mx &a)
        {
            assert((m_start + a.M) <= M);
            assert((n_start + a.N) <= N);
            
            for(int m = 0; m < a.M; m++)
                for(int n = 0; n < a.N; n++)
                {
                    (*this)(m_start + m, n_start + n) = a(m, n);
                }
        }
        
        /*
         * permutation matrix:
         * add row i, multiplied with c, to row j
         */
        static mx permutation_add_rows(const mx &a, const float &c, const unsigned &i, const unsigned &j)
        {
            mx p(a.M, a.N);
            
            // ...
            
            return p; 
        }
        
        /*
         * permutation matrix:
         * swap rows i and j
         */
        static mx permutation_swap_rows(const mx &a, const unsigned &i, const unsigned &j)
        {
            mx p(a.M, a.N);
            
            // ...
            
            return p;
        }
        
        /*
         * permutation matrix:
         * multiply row i with c
         */
        static mx permutation_mult_rows(const mx &a, const unsigned &i, const float &c)
        {
            mx p(a.M, a.N);
            
            // ...
            
            return p;
        }
        
        void debug_print()
        {
            for(int m=0; m<M; m++)
            {
                for(int n=0; n<N; n++)
                {
                    //printf("%1.2f ", elem[n*M+m]);
                    printf("%d ", (int)elem[n*M+m]);
                }
                printf("\n");
            }
        }
        
        ~mx()
        {
            delete[] elem;
        }
    };
    
    
    class sqmx : public mx
    {
    public:
        
        sqmx(const unsigned &M) : mx(M, M) {}
    };
    
    
    
    /*
     general M x N matrix
     
     M: column size
     N: row size
     
     | 00 01 .. 0N |
     | 10 11 .. 1N |
     | .. .. .. .. |
     | M0 M1 .. MN |
     
     */
    
    template<typename T, int M, int N>
    class mat
    {
    public:
        int M_, N_;
        
        /* column major */
        
        T elem[N][M];
        
        /* identity */
        
        mat() : M_(M), N_(N)
        {
            for(int c=0; c<M; c++)
                for(int r=0; r<N; r++)
                {
                    elem[r][c] = (r==c ? 1 : 0);
                }
        }
        
        /* matrix mult: M x N * N x P => M x P */
        
        // #1
        template<int P>
        mat<T, M, P> mult(const mat<T, N, P> &m)
        {
            //printf("%d, %d, %d\n", M, N, P);
            return mat<T, M, P>();
        }
        
        // #2
        template<int J, int K, int L>
        static mat<T, J, L> mult2(const mat<T, J, K> &m0, const mat<T, K, L> &m1)
        {
            //printf("%d, %d, %d\n", J, K, L);
            return mat<T, J, L>();
        }
        
        // #3
        template<int P>
        static mat<T, M, N> mult3(const mat<T, M, P> &m0, const mat<T, P, N> &m1)
        {
            //printf("%d, %d, %d\n", M, P, N);
            return mat<T, M, N>();
        }
        
        void debug_print()
        {
            for(int c=0; c<M; c++)
            {
                for(int r=0; r<N; r++)
                {
                    printf("%1.3f ", (float)elem[r][c]);
                }
                printf("\n");
            }
        }
    };
    
    
    /*
     square matrix
     
     note on template class inheriting from another template class:
     need this-> to access memebers of base due to compiler namespace look up rules
     e.g. this->elem[][]
     http://stackoverflow.com/questions/3799495/template-inheritance-c
     */
    
    template<typename T, int N>
    class sqmat : public mat<T, N, N>
    {
    public:
        
        /* identity */
        
        sqmat() : mat<T, N, N>() {}
    };
    
}

#endif /* MAT3_H */
