//
//  Camera.hpp
//  MetalTest
//
//  Created by Trystan Larey-Williams on 6/16/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

#pragma once

#include <simd/simd.h>

class CameraMetal {
public:
    matrix_double4x4 getViewMatrix();
    void setViewMatrix( const matrix_double4x4& );
    void setViewMatrixAsLookAt();
    
private:
    matrix_double4x4 viewMatrix;
};


