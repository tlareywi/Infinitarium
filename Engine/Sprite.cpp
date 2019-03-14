//
//  Sprite.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 3/13/19.
//

#include "Sprite.hpp"
#include "DataPack.hpp"

Sprite::Sprite( float nativeAspect ) {
   
}

void Sprite::prepare( IRenderContext& context ) {
   if( !dirty ) return;
   
   renderCommand = IRenderCommand::Create();
   
   renderCommand->setVertexCount( 1 );
   renderCommand->setInstanceCount( numPoints );
   renderCommand->setPrimitiveType( IRenderCommand::Point );
   
   for( auto& dataBuf : vertexBuffers ) {
      std::shared_ptr<IDataBuffer> buf = IDataBuffer::Create( context );
      buf->set( dataBuf.second );
      std::cout<<"Adding data buffer "<<dataBuf.first<<std::endl;
      renderCommand->add( buf );
   }
   
   IRenderable::prepare( context );
}



