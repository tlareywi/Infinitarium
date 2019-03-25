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
   
   virtual ~IRenderCommand() {}
   
   static std::shared_ptr<IRenderCommand> Create();
   
   void add( std::shared_ptr<IDataBuffer>& );
   void add( std::shared_ptr<ITexture>& );
   
   void setInstanceCount( unsigned int c ) {
      instanceCount = c;
   }
   void setVertexCount( unsigned int c ) {
      vertexCount = c;
   }
   
   virtual void encode( IRenderPass&, const IRenderState& ) = 0;
   virtual void setPrimitiveType( PrimitiveType ) = 0;
   
protected:
   std::vector<std::shared_ptr<IDataBuffer>> dataBuffers;
   std::vector<std::shared_ptr<ITexture>> textures;
   unsigned int instanceCount;
   unsigned int vertexCount;
};
