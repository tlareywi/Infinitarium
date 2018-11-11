//
//  PointCloud.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#include "PointCloud.hpp"
#include "DataBuffer.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_GUID(PointCloud, "PointCloud")

void PointCloud::addVertexBuffer( DataPackContainer& datapack, const std::string& name ) {
   std::visit( [this, name](auto& e) {
      vertexBuffers.emplace( name, std::move(e) );
   }, datapack );
}

void PointCloud::prepare( IRenderContext& context ) {
   for( auto& dataBuf : vertexBuffers ) {
      std::shared_ptr<IDataBuffer> buf = IDataBuffer::Create( context );
      buf->set( dataBuf.second );
      std::cout<<"Adding data buffer "<<dataBuf.first<<std::endl;
      renderCommand->add( buf );
   }
   
   std::shared_ptr<IRenderProgram> shader = IRenderProgram::Create();
   pipelineState->setProgram( shader );
   pipelineState->prepare( context );
   
   IRenderable::prepare( context );
   
   dirty = false;
}

void PointCloud::render( IRenderPass& renderPass ) { // TODO: Move to base class?
   if( dirty ) prepare( *(renderPass.renderContext) );
   
   pipelineState->apply();
   renderCommand->encode( renderPass, *pipelineState );
}


