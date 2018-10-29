//
//  PipelineState.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "RenderState.hpp"

extern std::shared_ptr<IRenderState> CreateMetalRenderState();

std::shared_ptr<IRenderState> IRenderState::Create() {
   return CreateMetalRenderState();
}
