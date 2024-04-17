//
//  matsys.h
//  tau3d
//
//  Created by Carl Johan Gribel on 2012-12-05.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef tau3d_matsys_h
#define tau3d_matsys_h

//#include <cstdio>
#include "math.h"
#include "vec.h"

namespace linalg
{
    
    /*
     solve_LU
     solve AX=B using LU decomposition, return X
     
     full pivoting:
     PAQ = LU (A permuted left & right)
     
     void solve_LU(A, B)
     {
     find P, L, U:
     P = permutation matrix P (swapping of rows & columns)
     L = identity-left-triangular L (elimination?)
     U = right-step-matrix U (A with lower tri elements eliminated)
     PA = LU
     
     algorithm:
     Doolittle,
     http://en.wikipedia.org/wiki/LU_decomposition#Doolittle_algorithm
     
     AX = B ->
     PAX = PB ->
     LUX = PB
     solve for X (structure of LU yields one element at a time)
     }
     
     */
    
    /*
     * decompose A into LU using Doolittle decomposition
     *
     * TODO: make this description full and comprihensible
     *
     * A: square, non-singular matrix (or just allow ero-rows and return the rank?)
     * L: lower triangular matrix (vänster-identitetsmatris)
     * U: upper triangular matrix (hger-trapp-matris)
     *
     * pivoting = use max column element as pivot, not just any != 0
     * if using pivoting, need to return permutation matrix (or something
     *  corresponding to that) since B (in AX=B) needs to be permuted as well before system can be solved
     */
    static void doolitte(float *A, const int &n)
    {
        
    }
    
    static void doolittle_pivoting_decomposition(const mx &A, mx &L, mx &U)
    {
        assert(A.M == A.N);         // A square
        assert(L.M == L.M && L.N == L.N);  // L sqaure and same dimension as A
        assert(U.M == U.M && U.N == U.N);  // U sqaure and same dimension as A
        
        unsigned N = A.M;
        
        for (int i=0; i<N; i++)
        {
            for (int j=i; j<N; j++)
            {
                L(i, j) = 0;
            }
            
            for (int j=i; j<N; j++)
            {
                L(i, j) = 0;
            }
        }
    }
    
    /*
     * solve linear system AX = B
     *
     * TODO: explain more
     */
    static void solve_LU(const mx &A, const mx &B, mx &X)
    {
        assert(A.M == A.N);
        assert(A.N == B.M && B.N == 1);
        assert(A.N == X.M && X.N == 1);
        
        unsigned N = A.M;
        mx P = mx(N, N);
        mx L = mx(N, N); 
        
        // doolittle to find L, U
        
        // permute? i.e. pivot
        
        // solve for X using forward & then backward substitution
    }
}

#endif
