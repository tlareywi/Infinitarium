//
//  RenderCommand.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#include "RenderCommand.hpp"

extern std::shared_ptr<IRenderCommand> CreateMetalRenderCommand();

std::shared_ptr<IRenderCommand> IRenderCommand::Create() {
   return CreateMetalRenderCommand();
}

void IRenderCommand::add( std::shared_ptr<IDataBuffer>& buf ) {
   dataBuffers.push_back( buf );
}
