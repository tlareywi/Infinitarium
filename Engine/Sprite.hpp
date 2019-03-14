//
//  Sprite.hpp
//  IEPlatform
//
//  Created by Trystan Larey-Williams on 3/13/19.
//

#pragma once

#include "Renderable.hpp"
#include "DataPack.hpp"

class Sprite : public IRenderable {
public:
   Sprite( float nativeAspect );
   
   void prepare( IRenderContext& context ) override;
   
   void setImage( const std::string& filename );
   
protected:
   
private:
   DataPackContainer vertices;
   
};
