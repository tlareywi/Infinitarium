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

class IRenderCommand {
public:
   enum PrimitiveType {
      Point,
      Line,
      Triangle,
      TriangleStrip
   };
   
   IRenderCommand() : instanceCount(0), vertexCount(0) {

   }

   virtual ~IRenderCommand() {}
   
   static std::shared_ptr<IRenderCommand> Create();

   virtual void add(std::shared_ptr<IDataBuffer>& buf) {
       dataBuffers.push_back(buf);
       if (buf->getUsage() == IDataBuffer::Usage::Pick)
           _pickIndx = dataBuffers.size() - 1;
   }

   virtual void add(std::shared_ptr<ITexture>& t) {
       textures.push_back(t);
   }

   void getBufferData( unsigned int indx, const glm::uvec4& rect, size_t srcWidth, size_t bytesPerUnit, void* out ) {
       dataBuffers[indx]->getData(rect, srcWidth, bytesPerUnit, out);
   }
   
   void setInstanceCount( unsigned int c ) {
      instanceCount = c;
   }
   void setVertexCount( unsigned int c ) {
      vertexCount = c;
   }
   unsigned int pickIndx() {
       return _pickIndx;
   }
   
   virtual void encode( IRenderPass&, IRenderState& ) = 0;
   virtual void setPrimitiveType( PrimitiveType ) = 0;
   
protected:
   std::vector<std::shared_ptr<IDataBuffer>> dataBuffers;
   std::vector<std::shared_ptr<ITexture>> textures;
   unsigned int instanceCount;
   unsigned int vertexCount;
   unsigned int _pickIndx{0};
};
