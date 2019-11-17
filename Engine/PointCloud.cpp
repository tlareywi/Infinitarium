//
//  PointCloud.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#include "PointCloud.hpp"
#include "DataBuffer.hpp"
#include "PythonBridge.hpp"

#include "../config.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PointCloud)

PointCloud::PointCloud() {
}

void PointCloud::addVertexBuffer( DataPackContainer& datapack, const std::string& name ) {
   std::visit( [this, name](auto& e) {
      vertexBuffers.emplace( name, std::move(e) );
   }, datapack );
}

void PointCloud::prepare( IRenderContext& context ) {
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




