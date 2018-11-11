//
//  PipelineState.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "RenderState.hpp"
#include "Module.hpp"

std::shared_ptr<IRenderState> IRenderState::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createRenderState();
}
