//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
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
   Sprite() {}
   Sprite( float nativeAspect );
   
   void prepare( IRenderContext& context ) override;
   
private:   
   static boost::uuids::uuid geometryId;
   
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive &, unsigned int );
};

BOOST_CLASS_EXPORT_KEY(Sprite);

class BloomSprite : public Sprite {
public:
    BloomSprite() {}
    void prepare(IRenderContext& context) override;

private:
    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive&, unsigned int);
};

BOOST_CLASS_EXPORT_KEY(BloomSprite);