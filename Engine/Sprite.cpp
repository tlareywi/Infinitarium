//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Sprite.hpp"
#include "DataPack.hpp"
#include "UniformType.hpp"
#include "Application.hpp"

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
   
   IRenderable::prepare( context );
}

template<class Archive> void Sprite::serialize(Archive& ar, unsigned int version) {
      std::cout<<"Serializing Sprite"<<std::endl;
      boost::serialization::void_cast_register<Sprite,IRenderable>();
      ar & boost::serialization::base_object<IRenderable>(*this);
}

///
/// BloomSprite: Subscribes to bloomCenter event published by SpheroidEmitter for radial blur (bloom) effect and passes uniform to shader
/// 

BOOST_CLASS_EXPORT_IMPLEMENT(BloomSprite)

void BloomSprite::prepare(IRenderContext& context) {
    setUniform( "bloomCenter", Uniform(UniformType(glm::vec2(0.0, 0.0))) );

    auto fun = [this](glm::vec2 pos) {
        updateUniform( "bloomCenter", Uniform(UniformType(pos)) );
    };
    std::shared_ptr<IDelegate> delegate = std::make_shared<EventDelegate<decltype(fun), glm::vec2>>(fun);
    IApplication::Create()->subscribe("BloomCenter", delegate);

    Sprite::prepare(context);
}

template<class Archive> void BloomSprite::serialize(Archive& ar, unsigned int version) {
    std::cout << "Serializing BloomSprite" << std::endl;
    boost::serialization::void_cast_register<BloomSprite, Sprite>();
    ar& boost::serialization::base_object<Sprite>(*this);
}




