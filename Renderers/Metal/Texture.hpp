//
//  Texture.hpp
//  IERenderer
//
//  Created by Trystan (Home) on 2/20/19.
//

#pragma once

#include "../../Engine/Texture.hpp"

#import <Metal/Metal.h>

class MetalTexture : public ITexture {
public:
   MetalTexture( glm::uvec2, ITexture::Format );
private:
   id<MTLTexture> texture;
};
