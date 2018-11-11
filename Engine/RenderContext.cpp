//
//  RenderContext.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#include "RenderContext.hpp"
#include "Module.hpp"

std::shared_ptr<IRenderContext> IRenderContext::Create( unsigned int x, unsigned int y, unsigned int w, unsigned int h ) {
   return ModuleFactory<RendererFactory>::Instance()->createRenderContext(x, y, w, h);
}
