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
   Sprite() {
       setProgram("sprite");
   }
   Sprite( float nativeAspect );
   
   void prepare( IRenderContext& context ) override;
   
private:   
   static std::shared_ptr<IDataBuffer> quad;
   
   friend class boost::serialization::access;
   template<class Archive> friend void boost::serialization::serialize( Archive &, Sprite&, unsigned int );
};
