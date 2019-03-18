//
//  Sprite.hpp
//  IEPlatform
//
//  Created by Trystan Larey-Williams on 3/13/19.
//

#pragma once

#include "MotionControllerOrbit.hpp"
#include "Renderable.hpp"
#include "DataPack.hpp"
#include "Texture.hpp"

class Sprite : public IRenderable {
public:
   Sprite( float nativeAspect );
   
   void prepare( IRenderContext& context ) override;
   
   void setTexture( const std::shared_ptr<ITexture>& );
   void setMotionController( const std::shared_ptr<ITexture>& );
   
private:
   DataPackContainer quad;
   std::shared_ptr<ITexture> texture;
};
