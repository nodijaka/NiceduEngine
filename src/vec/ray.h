//
//  ray.h
//  tau3d
//
//  Created by Carl Johan Gribel on 2014-10-09.
//
//

#ifndef tau3d_ray_h
#define tau3d_ray_h

#include "vec.h"

using linalg::float3;

class ray_t
{
public:
    
    float3 origo, dir;
    float znear;
    float3 r;             // hit point in body space (anchor point)

    ray_t() {}
    
    ray_t(float3 &origo, float3 &dir) : origo(origo), dir(dir), znear(fINF) {}
    
//    operator bool() { return body != NULL; }
};

#endif
