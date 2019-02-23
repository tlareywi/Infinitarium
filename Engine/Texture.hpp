//
//  Texture.hpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 2/20/19.
//

#pragma once

#include <memory>

#include <glm/gtc/matrix_transform.hpp>

class ITexture {
public:
   enum Format {
      BRGA8,
      BRGA8_sRGB,
      RU32
   };
   
   static std::shared_ptr<ITexture> Create( glm::uvec2, Format );
};

class IRenderTarget : public ITexture {
public:
   enum Type {
      Color,
      Depth
   };
   
   static std::shared_ptr<IRenderTarget> Create( glm::uvec2, Format );
};
