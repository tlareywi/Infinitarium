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

void IRenderCommand::add( std::shared_ptr<IDataBuffer>& buf ) {
   dataBuffers.push_back( buf );
}

void IRenderCommand::add( std::shared_ptr<ITexture>& t ) {
   textures.push_back( t );
}
