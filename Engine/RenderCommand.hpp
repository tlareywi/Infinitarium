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
   }

   virtual void add(std::shared_ptr<ITexture>& t) {
       textures.push_back(t);
   }
   
   void setInstanceCount( unsigned int c ) {
      instanceCount = c;
   }
   void setVertexCount( unsigned int c ) {
      vertexCount = c;
   }
   
   virtual void encode( IRenderPass&, IRenderState& ) = 0;
   virtual void setPrimitiveType( PrimitiveType ) = 0;
   
protected:
   std::vector<std::shared_ptr<IDataBuffer>> dataBuffers;
   std::vector<std::shared_ptr<ITexture>> textures;
   unsigned int instanceCount;
   unsigned int vertexCount;
};
