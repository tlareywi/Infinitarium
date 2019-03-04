//
//  RenderPass.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "RenderPass.hpp"
#include "Module.hpp"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

std::shared_ptr<IRenderPass> IRenderPass::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createRenderPass();
}



