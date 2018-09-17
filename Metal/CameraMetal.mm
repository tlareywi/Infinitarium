//
//  Camera.cpp
//  MetalTest
//
//  Created by Trystan Larey-Williams on 6/16/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

#include "CameraMetal.h"

matrix_double4x4 CameraMetal::getViewMatrix() {
    return viewMatrix;
}

void CameraMetal::setViewMatrix( const matrix_double4x4& ) {
    
}

void CameraMetal::setViewMatrixAsLookAt() {
    
}
