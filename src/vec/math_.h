
/*
 * Tau3D Dynamics 
 * Carl Johan Gribel (c) 2011, cjgribel@gmail.com
 *
 * nov 12 - simplefloor, floor, smoothstep, step, pulse, mod, gammacorrect from book "Texturing & Modeling"
 */

#pragma once
#ifndef MATH_H
#define MATH_H

#include <math.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>

#ifndef DEBUG
#define NDEBUG          /* NDEBUG will disable assert */
#endif
#include <cassert>

#define fPI			3.141592653f
#define fINF		3.4028234e38
#define fNINF		-3.4028234e38
#define fTO_RAD		(fPI/180.0f)
#define fTO_DEG		(180.0f/fPI)

#define simplefloor(x) ((double)((long)(x)-((x)<0.0)))
//#define floor(x) ((tempdoub=(x))<0.0 ? ((long)tempdoub)-1L : (long)tempdoub)

//
// Factorial (compile-time)
// Usage:
// int a = factorial<0>(); // 1
// int a = factorial<4>();; // 24
//

template<int N>
struct Factorial
{
    enum { value = N * Factorial<N-1>::value };
};

template<>
struct Factorial<0>
{
    enum { value = 1 };
};

template<int N>
inline int factorial()
{
    return Factorial<N>::value;
}

//
// Binomial coefficient (compile-time)
//
// Usage:
// int b = binomial<2,2>();
//
// Can be used e.g. for evaluating Bernstein polynomials (BezierSpline.h)
//
// Recursive form, wiki:    https://en.wikipedia.org/wiki/Binomial_coefficient#Recursive_formula
// Inspiration, go-lambda:  http://go-lambda.blogspot.se/2012/02/template-for-binomial-coefficients-in-c.html

template<int n, int k>
struct Binomial
{
    enum { value = Binomial<n-1, k-1>::value + Binomial<n-1, k>::value };
};

template<> struct Binomial<0, 0>
{
    enum { value = 1 };
};

template<int n>
struct Binomial<n, 0>
{
    enum { value = 1 };
};

template<int n>
struct Binomial<n, n>
{
    enum { value = 1 };
};

// Recursive form
template<int n, int k>
inline int binomial()
{
    return Binomial<n, k>::value;
}

// Factorial form
template<int n, int k>
inline int binomial_factorial()
{
    return factorial<n>() / ( factorial<k>()*factorial<n-k>() );
}

//inline float rnd(const float &min, const float &max)
//{
//    return min + (float)rand()/RAND_MAX*(max-min);
//}

//
// Random value
//
template<class T>
inline T rnd(const T &min, const T &max)
{
    return min + (T)rand()/RAND_MAX*(max-min);
}

//
// Clamp
//
template<typename T>
inline T clamp(const T &a, const T &min, const T &max)
{
    return std::max<T>(min, std::min<T>(max, a));
}

//
// Modulus
// Also handles negative values
//
inline float mod(float a, float b)
{
    int n = (int)(a/b);
    a -= n*b;
    if (a < 0)
        a += b;

    return a;
}

//
// Gamma-correct
//
inline float gammacorrect(const float &gamma, const float &x) { return powf(x, 1.0f/gamma); }


#endif /* MATH_H */