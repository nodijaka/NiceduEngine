
/*
 2-4D vectors and algebra
 v 1.0
 
 Carl Johan Gribel (c) 2011, cjgribel@gmail.com
 
 Templated definitions in cpp-file requires explicit template specialisation:
 http://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file
 */

#include "vec.h"
#include "mat.h"
#include <float.h>

namespace linalg
{
    
    template <class T>
    vec4<T> vec3<T>::xyz0() const
    {
        return vec4<T>(x, y, z, 0.0);
    }
    // explicit template specialisation for <float>
    template vec4<float> vec3<float>::xyz0() const;
    
    template <class T>
    vec4<T> vec3<T>::xyz1() const
    {
        return vec4<T>(x, y, z, 1.0);
    }
    // explicit template specialisation for <float>
    template vec4<float> vec3<float>::xyz1() const;
    
    /*
     row vector * matrix = row vector
     */
    template <class T>
    vec3<T> vec3<T>::operator *(const mat3<T> &m) const
    {
        return vec3<T>(x*m.m11 + y*m.m21 + z*m.m31,
                       x*m.m12 + y*m.m22 + z*m.m32,
                       x*m.m13 + y*m.m23 + z*m.m33);
    }
    // explicit template specialisation for <float>
    template vec3<float> vec3<float>::operator *(const mat3<float> &m) const;

    
    //
    // Equality operator
    //
    template <class T>
    bool vec3<T>::operator ==(const vec3<T> &v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }
    
    // Specialisations
    
    template <>
    bool vec3<int>::operator == (const vec3<int>& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    
    template <>
    bool vec3<unsigned>::operator == (const vec3<unsigned>& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    
    // Other (more robust) epsilon strategies
    // https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
    //
    template <>
    bool vec3<float>::operator == (const vec3<float>& rhs) const
    {
        return  fabs(x-rhs.x) <= FLT_EPSILON &&
                fabs(y-rhs.y) <= FLT_EPSILON &&
                fabs(z-rhs.z) <= FLT_EPSILON;
    }
    
//    /*
//     * vec equality operator
//     */

//    // explicit template specialisation for <float>
//    template bool vec3<int>::operator ==(const vec3<int> &m) const;
//    template bool vec3<float>::operator ==(const vec3<float> &m) const;
//    
//    template bool operator == (const vec3<T>& a, const long3& b) {
//        return a.x == b.x && a.y == b.y && a.z == b.z;
//    }
//    template vec3<float> vec3<float>::operator *(const mat3<float> &m) const;
    
    //
    //                | a |             | ad ae af |
    // outer product: | b | | d e f | = | bd be bf |
    //                | c |             | cd ce cf |
    //
    template <class T>
    mat3<T> vec3<T>::outer_product(const vec3<T> &v) const
    {
        return mat3<T>(*this * v.x, *this * v.y, *this * v.z);
        //    return mat3<T>::from_basis(*this * v.x, *this * v.y, *this * v.z);
    }
    // explicit template specialisation for <float>
    template mat3<float> vec3<float>::outer_product(const vec3<float> &v) const;
    
}
