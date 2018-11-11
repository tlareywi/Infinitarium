//
//  Texture.hpp
//  Infinitaruim-Engine
//
//  Created by Trystan (Home) on 10/12/18.
//

#pragma once

#include "DataPack.hpp"
#include "RenderContext.hpp"

#include <memory>

class IDataBuffer;

class IDataBuffer {
public:
   static std::shared_ptr<IDataBuffer> Create( IRenderContext& );
   
   virtual void set( DataPackContainer& ) = 0;
   virtual void reserve( unsigned int sizeBytes ) = 0;
   virtual void set( const void* const, unsigned int sizeBytes ) = 0;
};
