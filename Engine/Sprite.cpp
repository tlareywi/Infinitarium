//
//  Sprite.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 3/13/19.
//

#include "Sprite.hpp"
#include "DataPack.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(Sprite)

std::shared_ptr<IDataBuffer> Sprite::quad = nullptr;

struct SpriteVertex {
   glm::fvec3 coord;
   glm::fvec2 textCoord;
};

static const SpriteVertex quadVerts[] =
{
   // Pixel positions, Texture coordinates
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
   if( !dirty ) return;
   
   renderCommand = IRenderCommand::Create();
   
   renderCommand->setVertexCount( 6 );
   renderCommand->setInstanceCount( 1 );
   renderCommand->setPrimitiveType( IRenderCommand::Triangle );
   
   // Shared data buffer across all sprite instances.
   // TODO: need to design a general way to cache/reuse databuffers
   if( !quad ) {
      quad = IDataBuffer::Create( context );
      quad->set( quadVerts, sizeof(quadVerts) );
      quad->commit();
   }
   
   renderCommand->add( quad );
   
   IRenderable::prepare( context );
}

template<class Archive> void Sprite::serialize(Archive& ar, unsigned int version) {
      std::cout<<"Serializing Sprite"<<std::endl;
      boost::serialization::void_cast_register<Sprite,IRenderable>();
      ar & boost::serialization::make_nvp("IRenderable", boost::serialization::base_object<IRenderable>(*this));
}



