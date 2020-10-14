//
//  RenderCommand.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "RenderCommand.hpp"
#include "Module.hpp"

std::shared_ptr<IRenderCommand> IRenderCommand::Create() {
   return ModuleFactory<RendererFactory>::Instance()->createRenderCommand();
}

