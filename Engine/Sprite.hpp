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
   Sprite() {}
   Sprite( float nativeAspect );
   
   void prepare( IRenderContext& context ) override;
   
   void setTexture( const std::shared_ptr<ITexture>& );
   void setMotionController( const std::shared_ptr<ITexture>& );
   
   template<class Archive> void save( Archive& ) const;
   template<class Archive> void load( Archive& );
   
private:
   std::shared_ptr<ITexture> texture;
   
   static std::shared_ptr<IDataBuffer> quad;
   
   friend class boost::serialization::access;
   template<class Archive> friend void boost::serialization::serialize( Archive &, Sprite&, unsigned int );
};
