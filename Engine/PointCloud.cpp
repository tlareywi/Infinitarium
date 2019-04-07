//
//  PointCloud.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#include "PointCloud.hpp"
#include "DataBuffer.hpp"
#include "PythonBridge.hpp"
#include "Delegate.hpp"
#include "Application.hpp"

#include "../config.h"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(PointCloud)

PointCloud::PointCloud() : pickCoords(glm::uvec2(0.0)) {
   // Subscribe to picking events.
   auto fun = [this]( const glm::uvec2& screenCoords ) {
      pickCoords = screenCoords;
   };
   std::shared_ptr<IDelegate> delegate = std::make_shared<EventDelegate<decltype(fun), const glm::uvec2&>>( fun );
   IApplication::Create()->subscribe("picking", delegate);
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

void PointCloud::render( IRenderPass& renderPass ) {
   IRenderable::render(renderPass);
   
   if( pickCoords != glm::uvec2(0.0) ) {
      uint32_t buf[100];
      memset( buf, 0, sizeof(buf) );
      glm::uvec4 rect( pickCoords.x - 5, pickCoords.y - 5, 10, 10 );
      renderPass.getData( 1, rect, (void*)buf );
      pickCoords = glm::uvec2(0.0);
   }
}




