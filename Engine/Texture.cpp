//
//  Texture.cpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 2/20/19.
//

#include "Texture.hpp"
#include "Module.hpp"

std::shared_ptr<ITexture> ITexture::Create( glm::uvec2 dim, Format format ) {
   return ModuleFactory<RenderFactory>::Instance()->createTexture( dim, format );
}
