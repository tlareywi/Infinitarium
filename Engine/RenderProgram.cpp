//
//  RenderProgram.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/21/18.
//

#include "RenderProgram.hpp"

extern std::shared_ptr<IRenderProgram> CreateMetalRenderProgram();

std::shared_ptr<IRenderProgram> IRenderProgram::Create() {
   return CreateMetalRenderProgram();
}


