//
//  RenderPass.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "RenderPass.hpp"

extern std::shared_ptr<IRenderPass> CreateMetalRenderPass();

std::shared_ptr<IRenderPass> IRenderPass::Create() {
   return CreateMetalRenderPass();
}


