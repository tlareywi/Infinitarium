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
#include "CoordinateSystem.hpp"

#include "../config.h"

BOOST_CLASS_EXPORT_IMPLEMENT(PointCloud)

PointCloud::PointCloud() : pickCoords(glm::uvec2(0)) {
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
   static bool init{ false };
   
   if (!init) {
       renderCommand = IRenderCommand::Create();

       renderCommand->setVertexCount(1);
       renderCommand->setInstanceCount(numPoints);
       renderCommand->setPrimitiveType(IRenderCommand::Point);

       for (auto& dataBuf : vertexBuffers) {
           std::shared_ptr<IDataBuffer> buf = IDataBuffer::Create(context);
           buf->setUsage(IDataBuffer::Usage::Storage);
           buf->set(dataBuf.second);
           std::cout << "Adding data buffer " << dataBuf.first << std::endl;
           renderCommand->add(buf);
       }

       init = true;
   }
   
   IRenderable::prepare( context );
}

void PointCloud::update( UpdateParams& params ) {
   motionController = params.getCamera().getMotionController();
   IRenderable::update( params );
}

#pragma warning( push )
#pragma warning( disable : 4244 ) // Disable type conversion warnings for assignment from variant type

void PointCloud::render( IRenderPass& renderPass ) {
   IRenderable::render(renderPass);
   
   if( pickCoords != glm::uvec2(0) ) {
      // Hmm, whether or not to have this as a post render op has interesting implications. As a non-post
      // render op, it effectively forces pick buffer evaluation before other things have drawn. As a post
      // op, everything on this camera will have drawn when this is called.
      std::function<void(IRenderPass&)> postRenderOp = [this](IRenderPass& renderPass) {
		 const unsigned short PickBuffer{ 1 };
		 const unsigned int szx{10}, szy{10};
         float buf[szx*szy];
         memset( buf, 0, sizeof(buf) );
         std::cout<<"Pick coord "<<pickCoords.x<<", "<<pickCoords.y<<std::endl;
         glm::uvec4 rect( pickCoords.x - 5, pickCoords.y + 5, szx, szy );
         renderPass.getData( PickBuffer, rect, (void*)buf );
         // OK, so we have access to the buffers with the J2000 data right here! Just need to generalize looking
         // up fields in the buffer. Then we can seed an instance of a slectable object with this data.
         float maxMag{30.0f};
         unsigned int pick{0};
         
         for( auto& val : buf ) {
            if( val != 0 ) { // TODO init buffer to max(uint) and make that the uninitialized value.
               float mag;
               
               { auto p = vertexBuffers.find(std::string("magnitude"));
				 auto& v = val;
                 std::visit( [v, &mag](auto& e) {
                  mag = e[v];
               }, p->second ); }
               
               if( mag < maxMag ) {
                  maxMag = mag;
                  pick = val;
               }
            }
         }
         
         glm::vec3 pos, color;
         { auto p = vertexBuffers.find(std::string("position"));
            std::visit( [pick, &pos](auto& e) {
               pos = glm::vec3( e[3*pick], e[3*pick+1], e[3*pick+2] ); // Maybe define name + stride in Py to generalize?
            }, p->second ); }
         
         { auto p = vertexBuffers.find(std::string("color"));
            std::visit( [pick, &color](auto& e) {
               color = glm::vec3( e[3*pick], e[3*pick+1], e[3*pick+2] );
            }, p->second ); }
         
         std::cout<<"Pick ID "<<pick<<", mag "<<maxMag<<" pos "<<pos.x<< " "<<pos.y <<" "<< pos.z<<std::endl;
         
         UniversalPoint center( pos, UniversalPoint::Parsec );
         std::shared_ptr<CoordinateSystem> system = std::make_shared<CoordinateSystem>( center, 40.0, UniversalPoint::AstronomicalUnit );
         addChild( system );
         motionController->select( system );
         
         pickCoords = glm::uvec2(0.0);
      };
      
      renderPass.postRenderOperation( postRenderOp );
   }
}

#pragma warning( pop )

