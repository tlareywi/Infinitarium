//
//  Sprite.cpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 3/13/19.
//

#include "Sprite.hpp"
#include "DataPack.hpp"

struct SpriteVertex {
   glm::fvec2 coord;
   glm::fvec2 textCoord;
};

static const SpriteVertex quadVerts[] =
{
   // Pixel positions, Texture coordinates
   { {  1.f,  -1.f },  { 1.f, 0.f } },
   { { -1.f,  -1.f },  { 0.f, 0.f } },
   { { -1.f,   1.f },  { 0.f, 1.f } },
   
   { {  1.f,  -1.f },  { 1.f, 0.f } },
   { { -1.f,   1.f },  { 0.f, 1.f } },
   { {  1.f,   1.f },  { 1.f, 1.f } },
};

Sprite::Sprite( float nativeAspect ) {
   
}

void Sprite::prepare( IRenderContext& context ) {
   if( !dirty ) return;
   
   renderCommand = IRenderCommand::Create();
   
   renderCommand->setVertexCount( 6 );
   renderCommand->setInstanceCount( 1 );
   renderCommand->setPrimitiveType( IRenderCommand::Triangle );
   
   // Hmmm, ths is not going to reuse the data buffer ... need to come up with a reuse mechanism here.
   std::shared_ptr<IDataBuffer> buf = IDataBuffer::Create( context );
   buf->set( quadVerts );
   renderCommand->add( buf );
   
   IRenderable::prepare( context );
}

void Sprite::setTexture( const std::shared_ptr<ITexture>& t ) {
   texture = t;
}

void Sprite::setMotionController( const std::shared_ptr<ITexture>& ) {
   
}



