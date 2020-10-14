//
//  Texture.cpp
//  Infinitaruim-Engine
//
//  Created by Trystan (Home) on 10/12/18.
//

#include "DataBuffer.hpp"
#include "Module.hpp"

std::shared_ptr<IDataBuffer> IDataBuffer::Create( IRenderContext& context ) {
   return ModuleFactory<RendererFactory>::Instance()->createDataBuffer( context );
}
