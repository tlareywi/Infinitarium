//
//  Sprite.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 3/13/19.
//

#include "Sprite.hpp"
#include "DataPack.hpp"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(Sprite)

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
   renderCommand->add( texture );
   
   IRenderable::prepare( context );
   texture->prepare( context );
}

void Sprite::setTexture( const std::shared_ptr<ITexture>& t ) {
   texture = t;
}

void Sprite::setMotionController( const std::shared_ptr<ITexture>& ) {
}

template<class Archive> void Sprite::save( Archive& ar ) const {
   std::unique_ptr<TextureProxy> t = std::make_unique<TextureProxy>(*texture);
   ar << t;
}

template<class Archive> void Sprite::load( Archive& ar ) {
   std::unique_ptr<TextureProxy> t;
   ar >> t;
   texture = ITexture::Clone( *t );
}

namespace boost { namespace serialization {
   template<class Archive> inline void load(Archive& ar, Sprite& t, unsigned int version) {
      std::cout<<"Loading Sprite"<<std::endl;
      ar >> boost::serialization::base_object<IRenderable>(t);
      t.load( ar );
   }
   template<class Archive> inline void save(Archive& ar, const Sprite& t, unsigned int version) {
      std::cout<<"Saving Sprite"<<std::endl;
      ar << boost::serialization::base_object<IRenderable>(t);
      t.save( ar );
   }
   template<class Archive> inline void serialize(Archive& ar, Sprite& t, unsigned int version) {
      boost::serialization::void_cast_register<Sprite,IRenderable>();
      boost::serialization::split_free(ar, t, version);
   }
}}



