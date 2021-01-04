//
//  Sprite.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 3/13/19.
//

#include "Sprite.hpp"
#include "DataPack.hpp"
#include "UniformType.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(Sprite)

boost::uuids::uuid Sprite::geometryId{ boost::uuids::random_generator()() };

struct SpriteVertex {
   glm::fvec3 coord;
   glm::fvec2 textCoord;
};

static const SpriteVertex quadVerts[] =
{
   // Vertex positions, Texture coordinates
   { {  1.f,  -1.f, 0.0 },  { 1.f, 0.f } },
   { { -1.f,  -1.f, 0.0 },  { 0.f, 0.f } },
   { { -1.f,   1.f, 0.0 },  { 0.f, 1.f } },
   
   { {  1.f,  -1.f, 0.0 },  { 1.f, 0.f } },
   { { -1.f,   1.f, 0.0 },  { 0.f, 1.f } },
   { {  1.f,   1.f, 0.0 },  { 1.f, 1.f } }
};

Sprite::Sprite( float nativeAspect ) {
   setProgram("sprite");
}

void Sprite::prepare( IRenderContext& context ) {   
   renderCommand = IRenderCommand::Create();
   
   renderCommand->setVertexCount( 6 );
   renderCommand->setInstanceCount( 1 );
   renderCommand->setPrimitiveType( IRenderCommand::PrimitiveType::Triangle );
   
   std::shared_ptr<IDataBuffer> quad = ObjectStore::instance().get(geometryId);
   if (!quad) {
       quad = IDataBuffer::Create(context);
       quad->set(quadVerts, sizeof(quadVerts));
       quad->setStride(sizeof(SpriteVertex));
       quad->commit();
       ObjectStore::instance().add(geometryId, quad);
   }

   {
       IRenderCommand::VertexAttribute attr{ IRenderCommand::AttributeType::Position, 0, offsetof(SpriteVertex, coord) };
       renderCommand->addVertexAttribute(attr);
   }
   {
       IRenderCommand::VertexAttribute attr{ IRenderCommand::AttributeType::UV, 1, offsetof(SpriteVertex, textCoord) };
       renderCommand->addVertexAttribute(attr);
   }
   renderCommand->add( quad );

   setUniform("scale", Uniform(UniformType(26.0f), UniformType(1.0f), UniformType(100.f)));
   
   IRenderable::prepare( context );
}

template<class Archive> void Sprite::serialize(Archive& ar, unsigned int version) {
      std::cout<<"Serializing Sprite"<<std::endl;
      boost::serialization::void_cast_register<Sprite,IRenderable>();
      ar & boost::serialization::make_nvp("IRenderable", boost::serialization::base_object<IRenderable>(*this));
}



