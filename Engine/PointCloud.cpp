//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
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
   definePickOp();

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

#pragma warning( push )
#pragma warning( disable : 4244 ) // Disable type conversion warnings for assignment from variant type

void PointCloud::definePickOp() {
    pickOp = [this](IRenderPass& renderPass) {
        const unsigned int szx{ 10 }, szy{ 10 };
        PickUnit buf[szx * szy];
        memset(buf, 0, sizeof(buf));

        glm::uvec4 rect( std::max(0, (int)pickCoords.x - 5), std::max(0, (int)pickCoords.y - 5), szx, szy );
        renderCommand->getBufferData(renderCommand->pickIndx(), rect, viewport.x, sizeof(PickUnit), static_cast<void*>(buf));

        float maxBrightness{ 0.0f };
        unsigned int pick{ 0 };

        // Find brightest star in pick region around click.
        for (auto& val : buf) {
            if (val.objId == 0)
                continue;

            if (val.brightness > maxBrightness) {
                maxBrightness = val.brightness;
                pick = val.objId;
            }
        }

        glm::vec3 pos, color;
        { auto p = vertexBuffers.find(std::string("position"));
        std::visit([pick, &pos](auto& e) {
            pos = glm::vec3(e[3 * pick], e[3 * pick + 1], e[3 * pick + 2]); // Maybe define name + stride in Py to generalize?
        }, p->second); }

        { auto p = vertexBuffers.find(std::string("color"));
        std::visit([pick, &color](auto& e) {
            color = glm::vec3(e[3 * pick], e[3 * pick + 1], e[3 * pick + 2]);
        }, p->second); }

        UniversalPoint center(pos, UniversalPoint::Parsec);
        // At least in our own solar system, orbits of commets go out ~50,000AU
        std::shared_ptr<CoordinateSystem> system = std::make_shared<CoordinateSystem>( center, 50000.0, UniversalPoint::AstronomicalUnit );
        system->setName("CoordinateSystem");
        addChild( system );
        motionController->select( system );

        pickCoords = glm::uvec2(0.0);
    };
}

#pragma warning( pop )

void PointCloud::prepare( IRenderContext& context ) {
   if (dirty) {
       renderCommand = IRenderCommand::Create();

       renderCommand->setVertexCount(1);
       renderCommand->setInstanceCount(numPoints);
       renderCommand->setPrimitiveType(IRenderCommand::PrimitiveType::Point);

       for (auto& dataBuf : vertexBuffers) {
           std::shared_ptr<IDataBuffer> buf = IDataBuffer::Create(context);
           buf->setUsage(IDataBuffer::Usage::Storage);
           buf->set(dataBuf.second);
           buf->commit();
           std::cout << "Adding data buffer " << dataBuf.first << std::endl;
           renderCommand->add(buf);
       }
   }
   
   IRenderable::prepare( context );
}

void PointCloud::update( UpdateParams& params ) {
   motionController = params.getCamera().getMotionController();
   IRenderable::update( params );
}

void PointCloud::render( IRenderPass& renderPass ) {
   IRenderable::render(renderPass);
   
   if( pickCoords != glm::uvec2(0) )
      renderPass.postRenderOperation( pickOp );
}


