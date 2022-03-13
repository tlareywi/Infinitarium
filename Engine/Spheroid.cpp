//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Spheroid.hpp"
#include "Application.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(Spheroid)
BOOST_CLASS_EXPORT_IMPLEMENT(Spheroid::SpheroidVertex)

boost::uuids::uuid Spheroid::geometryId{ boost::uuids::random_generator()() };

Spheroid::Spheroid( unsigned int meridian, unsigned int parallel, float oblateness, bool flipNormals ) { 
   geometry = std::make_shared<DataPack<SpheroidVertex>>( meridian * parallel );
   
   glm::dvec4 start = glm::dvec4(0, 0, 1, 1);
   glm::dvec3 rotationAxis = glm::dvec3(0, 0, -1);
   glm::dvec3 startRotationAxis = glm::dvec3(-1, 0, 0);
   
   unsigned short direction = 1;
   if (flipNormals)
      direction = -1;
   
   for( double t = 0; t < parallel; t++ ) {
      glm::dvec4 one = glm::rotate( glm::dmat4x4(1.0), t / parallel * glm::pi<double>(), startRotationAxis ) * start;
      glm::dvec4 two = glm::rotate( glm::dmat4x4(1.0), (t + 1) / parallel * glm::pi<double>(), startRotationAxis ) * start;
      
      for( double s = 0; s <= meridian; s++ ) {
         glm::dvec3 above = glm::rotate( glm::dmat4x4(1.0), s / meridian * 2.0 * glm::pi<double>() * direction, rotationAxis) * one;
         glm::dvec3 below = glm::rotate( glm::dmat4x4(1.0), s / meridian * 2.0 * glm::pi<double>() * direction, rotationAxis) * two;
         
         glm::dvec3 point = glm::dvec3( above.x, above.y, above.z * (1.0 - oblateness) );
         SpheroidVertex vert{};
         if( flipNormals || t > 0 || s > 0 ) {
            vert.vertex = point;
            vert.normal = point * (double)direction;
            if( flipNormals )
               vert.texCoord = glm::fvec2( s / meridian, t / parallel );
            else
               vert.texCoord = glm::fvec2( s / meridian, 1.0 - t / parallel );
            
             geometry->add( vert );
         }
         
         point = glm::dvec3( below.x, below.y, below.z * (1.0 - oblateness) );
         vert.vertex = point;
         vert.normal = point * (double)direction;
         if( flipNormals )
            vert.texCoord = glm::fvec2( s / meridian, (t + 1) / parallel );
         else
            vert.texCoord = glm::fvec2( s / meridian, 1.0 - (t + 1) / parallel );
         
         geometry->add( vert );
      }
   }
}

void Spheroid::prepare( IRenderContext& context ) {
   if( !dirty ) return;

   setCullMode(IRenderState::CullMode::Back);
   
   renderCommand = IRenderCommand::Create();
   
   renderCommand->setVertexCount( geometry->size() );
   renderCommand->setInstanceCount( 1 );
   renderCommand->setPrimitiveType( IRenderCommand::PrimitiveType::TriangleStrip );
   
   std::shared_ptr<IDataBuffer> spheroid = ObjectStore::instance().get(geometryId);
   if (!spheroid) {
       spheroid = IDataBuffer::Create(context);
       spheroid->set(geometry->get(), geometry->sizeBytes());
       spheroid->setStride(sizeof(Spheroid::SpheroidVertex));
       spheroid->commit();
       ObjectStore::instance().add(geometryId, spheroid);
   }
   
   {
       IRenderCommand::VertexAttribute attr{ IRenderCommand::AttributeType::Position, 0, offsetof(Spheroid::SpheroidVertex, vertex) };
       renderCommand->addVertexAttribute(attr);
   }
   {
       IRenderCommand::VertexAttribute attr{ IRenderCommand::AttributeType::Normal, 1, offsetof(Spheroid::SpheroidVertex, normal) };
       renderCommand->addVertexAttribute(attr);
   }
   {
       IRenderCommand::VertexAttribute attr{ IRenderCommand::AttributeType::UV, 2, offsetof(Spheroid::SpheroidVertex, texCoord) };
       renderCommand->addVertexAttribute(attr);
   }

   renderCommand->add( spheroid );
   
   IRenderable::prepare( context );
}

template<class Archive> void Spheroid::serialize(Archive& ar, unsigned int) {
    std::cout << "Serializing Spheroid" << std::endl;
    boost::serialization::void_cast_register<Spheroid,IRenderable>();
    ar & boost::serialization::base_object<IRenderable>(*this);
    ar & geometry;
}
   
template<class Archive> void Spheroid::SpheroidVertex::serialize(Archive& ar, unsigned int) {
	ar & vertex;
    ar & normal;
    ar & texCoord;
}

/// <summary>
/// SpheroidEmitter: Overrides update to notify listeners of object center coords for radial blur (bloom)
/// </summary>
/// <param name="params"></param>

BOOST_CLASS_EXPORT_IMPLEMENT(SpheroidEmitter)

void SpheroidEmitter::update(UpdateParams& params) {
    // Separate render passes may need this information (e.g. object center for radial blur). Fire event to update subscribers.
    glm::vec4 pos{ params.getMVP() * glm::vec4(0.0, 0.0, 0.0, 1.0) };
    pos /= pos.w;
    std::tuple<glm::vec2> args(glm::vec2(pos.x, pos.y));
    Event e(args);
    e.setName("BloomCenter");
    IApplication::Create()->invoke(e);

    Spheroid::update(params);
}

template<class Archive> void SpheroidEmitter::serialize(Archive& ar, unsigned int) {
    std::cout << "Serializing SpheroidEmitter" << std::endl;
    boost::serialization::void_cast_register<SpheroidEmitter, Spheroid>();
    ar & boost::serialization::base_object<Spheroid>(*this);
}

