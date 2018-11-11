//
//  RenderProgram.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/21/18.
//

#include "RenderProgram.hpp"
#include "Module.hpp"

std::shared_ptr<IRenderProgram> IRenderProgram::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createRenderProgram();
}


