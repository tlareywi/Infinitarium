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
#include "ObjectStore.hpp"

#include "../config.h"

class Sprite : public IRenderable {
public:
   Sprite() {
       setProgram("sprite");
   }
   Sprite( float nativeAspect );
   
   void prepare( IRenderContext& context ) override;
   
private:   
   static boost::uuids::uuid geometryId;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive &, unsigned int );
};

BOOST_CLASS_EXPORT_KEY(Sprite);
