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
   unsigned int instanceCount;
   unsigned int vertexCount;
};
