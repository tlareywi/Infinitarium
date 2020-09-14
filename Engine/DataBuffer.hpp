//
//  DataBuffer.hpp
//  Infinitaruim-Engine
//
//  Created by Trystan (Home) on 10/12/18.
//

#pragma once

#include "DataPack.hpp"
#include "RenderContext.hpp"
#include "Texture.hpp"

class IRenderTarget;

#include <memory>

class IDataBuffer {
public:
   virtual ~IDataBuffer() {}
   
   static std::shared_ptr<IDataBuffer> Create( IRenderContext& );

   enum class Usage {
	   VertexBuffer,
	   UniformBuffer
   };
   
   virtual void commit() = 0;
   virtual void set( DataPackContainer& ) = 0;
   virtual void reserve( unsigned int sizeBytes ) = 0;
   virtual void set( const void* const, unsigned int sizeBytes ) = 0;
   virtual void set( const void* const, unsigned int offset, unsigned int sizeBytes ) = 0;
   virtual void copy( IRenderTarget&, const glm::uvec4& ) = 0;
   virtual void getData( void* ) = 0;
  
   virtual uint32_t getStride() { return 1; }
   virtual uint32_t getFormat() { return 0; }

   void setUsage(IDataBuffer::Usage u) { usage = u; }
   IDataBuffer::Usage getUsage() { return usage; }

private:
	IDataBuffer::Usage usage;
};
