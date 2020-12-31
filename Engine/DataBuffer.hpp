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
	   VertexAttribute,
	   Uniform,
	   Storage,
	   Pick
   };
   
   virtual void commit() = 0;
   virtual void set( DataPackContainer& ) = 0;
   virtual void reserve( unsigned int sizeBytes ) = 0;
   virtual void set( const void* const, unsigned int sizeBytes ) = 0;
   virtual void set( const void* const, unsigned int offset, unsigned int sizeBytes ) = 0;
   virtual void copy( IRenderTarget&, const glm::uvec4& ) = 0;
   virtual void getData( void* ) = 0;
   virtual void getData( const glm::uvec4&, size_t, size_t, void* ) = 0;
  
   uint32_t getStride() { return stride; }
   void setStride(uint32_t sz) { stride = sz; }

   void setUsage(IDataBuffer::Usage u) { usage = u; }
   IDataBuffer::Usage getUsage() { return usage; }

private:
	uint32_t stride{ 1 };
	IDataBuffer::Usage usage{ Usage::VertexAttribute };
};


struct PickUnit {
	alignas(4) float brightness;
	alignas(4) uint32_t objId;
};
