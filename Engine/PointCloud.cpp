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
      // Hmm, whether or not to have this as a post render op has interesting implications. As a non-post
      // render op, it effectively forces pick buffer evaluation before other things have drawn. As a post
      // op, everything on this camera will have drawn when this is called.
      std::function<void(IRenderPass&)> postRenderOp = [this](IRenderPass& renderPass) {
         uint32_t buf[100];
         memset( buf, 0, sizeof(buf) );
         glm::uvec4 rect( pickCoords.x - 5, pickCoords.y - 5, 10, 10 );
         renderPass.getData( 1, rect, (void*)buf );
         // OK, so we have access to the buffers with the J2000 data right here! Just need to generalize looking
         // up fields in the buffer. Then we can seed an instance of a slectable object with this data.
         for( auto& val : buf ) {
            if( val != 0 ) { // TODO init buffer to max(uint) and make that the uninitialized value.
               glm::vec3 pos, color;
               float mag;
               { auto p = vertexBuffers.find(std::string("position"));
               std::visit( [val, &pos](auto& e) {
                  pos = glm::vec3( e[3*val], e[3*val+1], e[3*val+2] ); // Maybe define name + stride in Py to generalize?
               }, p->second ); }
               
               { auto p = vertexBuffers.find(std::string("color"));
               std::visit( [val, &color](auto& e) {
                  color = glm::vec3( e[3*val], e[3*val+1], e[3*val+2] );
               }, p->second ); }
               
               { auto p = vertexBuffers.find(std::string("magnitude"));
               std::visit( [val, &mag](auto& e) {
                  mag = e[3*val];
               }, p->second ); }
               
               break;
            }
            
            
         }
      };
      
      renderPass.postRenderOperation( postRenderOp );
   }
}




