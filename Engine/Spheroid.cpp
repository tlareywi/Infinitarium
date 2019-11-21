//
//  Spheroid.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 4/17/19.
//

#include "Spheroid.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(Spheroid)

Spheroid::Spheroid( unsigned int meridian, unsigned int parallel, float oblateness, bool flipNormals ) {
   
   geometry = std::make_shared<DataPack<SpheroidVertex>>( meridian * parallel );
   // Hmm maybe a geometry cache would be good with this pattern of IRenderables having a shared pointer to their vertex data?
   // So, request a matching DataPack type + size and if one exists return it otherewise create it? Also, then this cache would
   // be the serializer/deserializer of the DataPacks.
   
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
         SpheroidVertex vert;
         if( flipNormals || t > 0 || s > 0 ) {
            vert.vertex = point;
            vert.normal = point * (double)direction;
            if( flipNormals )
               vert.textCoord = glm::fvec2( s / meridian, t / parallel );
            else
               vert.textCoord = glm::fvec2( s / meridian, 1.0 - t / parallel );
            
             geometry->add( vert );
         }
         
         point = glm::dvec3( below.x, below.y, below.z * (1.0 - oblateness) );
         vert.vertex = point;
         vert.normal = point * (double)direction;
         if( flipNormals )
            vert.textCoord = glm::fvec2( s / meridian, (t + 1) / parallel );
         else
            vert.textCoord = glm::fvec2( s / meridian, 1.0 - (t + 1) / parallel );
         
         geometry->add( vert );
      }
   }
}

void Spheroid::prepare( IRenderContext& context ) {
   if( !dirty ) return;
   
   renderCommand = IRenderCommand::Create();
   
   renderCommand->setVertexCount( geometry->size() );
   renderCommand->setInstanceCount( 1 );
   renderCommand->setPrimitiveType( IRenderCommand::TriangleStrip );
   
   spheroid = IDataBuffer::Create( context );
   spheroid->set( geometry->get(), geometry->sizeBytes() );
   spheroid->commit();
   
   renderCommand->add( spheroid );
   
   IRenderable::prepare( context );
}

namespace boost { namespace serialization {
   template<class Archive> inline void serialize(Archive& ar, Spheroid& t, unsigned int version) {
      boost::serialization::void_cast_register<Spheroid,IRenderable>();
      ar & boost::serialization::base_object<IRenderable>(t);
      ar & t.geometry;
   }
   
   template<class Archive> inline void serialize(Archive& ar, Spheroid::SpheroidVertex& t, unsigned int version) {
      ar & t.vertex;
      ar & t.normal;
      ar & t.textCoord;
   }
}}

