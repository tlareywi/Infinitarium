//
//  RenderPass.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "RenderPass.hpp"
#include "Module.hpp"

std::shared_ptr<IRenderPass> IRenderPass::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createRenderPass();
}


