//
//  RenderCommand.hpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/20/18.
//

#pragma once

#include "DataBuffer.hpp"
#include "RenderPass.hpp"
#include "RenderState.hpp"
#include "Texture.hpp"

#include <map>

class IRenderCommand {
public:
   enum class PrimitiveType {
      Point,
      Line,
      Triangle,
      TriangleStrip
   };

   enum class AttributeType {
       Position,
       Normal,
       Color,
       UV
   };

   struct VertexAttribute {
       AttributeType type;
       uint8_t location;
       uint16_t offset;
   };
   
   IRenderCommand() : instanceCount(0), vertexCount(0), vertexStride(0) {

   }

   virtual ~IRenderCommand() {}
   
   static std::shared_ptr<IRenderCommand> Create();

   virtual void add(std::shared_ptr<IDataBuffer>& buf) {
       dataBuffers.push_back(buf);
       if (buf->getUsage() == IDataBuffer::Usage::Pick)
           _pickIndx = (uint16_t)(dataBuffers.size() - 1);
   }

   virtual void add(std::shared_ptr<ITexture>& t) {
       textures.push_back(t);
   }

   void addVertexAttribute( const VertexAttribute& attr ) {
       attributes.push_back(attr);
   }

   void getBufferData( uint32_t indx, const glm::uvec4& rect, size_t srcWidth, size_t bytesPerUnit, void* out ) {
       dataBuffers[indx]->getData(rect, srcWidth, bytesPerUnit, out);
   }
   
   void setInstanceCount( uint32_t c ) {
      instanceCount = c;
   }
   void setVertexCount( uint32_t c ) {
      vertexCount = c;
   }
   uint16_t pickIndx() {
       return _pickIndx;
   }
   
   virtual void encode( IRenderPass&, IRenderState& ) = 0;
   virtual void setPrimitiveType( PrimitiveType ) = 0;
   
protected:
   std::vector<std::shared_ptr<IDataBuffer>> dataBuffers;
   std::vector<std::shared_ptr<ITexture>> textures;
   std::vector<VertexAttribute> attributes;
   uint32_t instanceCount;
   uint32_t vertexCount;
   uint32_t vertexStride;
   uint16_t _pickIndx{0};
};
