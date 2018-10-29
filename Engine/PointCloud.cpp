//
//  PointCloud.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/6/18.
//

#include "PointCloud.hpp"
#include "DataBuffer.hpp"

#include <boost/serialization/export.hpp>

#include <glm/gtc/matrix_transform.hpp>

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
      std::cout<<"Adding data buffer "<<dataBuf.first<<std::endl;
      renderCommand->add( buf );
   }
   
   // Buffer for standard uniforms such as mvp matrix. This should probably move eventually. Not conceptually coupled to a renderable.
   std::shared_ptr<IDataBuffer> buf = IDataBuffer::Create();
   glm::mat4 projection = glm::perspective( glm::radians(89.0), 16.0 / 9.0, 0.0001, 100.0 );
   glm::mat4 view = glm::lookAt( glm::vec3(0,0,0), glm::vec3(0,0,1), glm::vec3(0,1,0) );
   glm::mat4 mvp = projection * view;
   buf->set ( &mvp, sizeof(mvp) );
   renderCommand->add( buf );
   
   std::shared_ptr<IRenderProgram> shader = IRenderProgram::Create();
   pipelineState->setProgram( shader );
   pipelineState->prepare();
   
   dirty = false;
}

void PointCloud::render( IRenderPass& renderPass ) { // TODO: Move to base class?
   if( dirty ) prepare();
   
   pipelineState->apply();
   renderCommand->encode( renderPass, *pipelineState );
}


