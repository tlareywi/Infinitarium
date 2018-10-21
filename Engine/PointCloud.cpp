//
//  PointCloud.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#include "PointCloud.hpp"
#include "DataBuffer.hpp"
#include "PipelineState.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_GUID(PointCloud, "PointCloud")

void PointCloud::addVertexBuffer( DataPackContainer& datapack, const std::string& name ) {
   std::visit( [this, name](auto& e) {
      vertexBuffers.emplace( name, std::move(e) );
   }, datapack );
}

void PointCloud::prepare() {
   for( auto& dataBuf : vertexBuffers ) {
      std::shared_ptr<IDataBuffer> buf = IDataBuffer::Create();
      buf->set( dataBuf.second );
      renderCommand->add( buf );
   }
   
   dirty = false;
}

void PointCloud::render( IRenderPass& renderPass ) { // TODO: Move to base class?
   if( dirty ) prepare();
   
   renderCommand->encode( renderPass );
}


