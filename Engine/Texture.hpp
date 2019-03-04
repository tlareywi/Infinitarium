//
//  Texture.hpp
//  InfinitariumEngine
//
//  Created by Trystan (Home) on 2/20/19.
//

#pragma once

#include <memory>

#include <glm/gtc/matrix_transform.hpp>

#include "RenderContext.hpp"

class ITexture {
public:
   enum Format {
      BRGA8,
      BRGA8_sRGB,
      RU32
   };
   
   ITexture( const glm::uvec2& d, Format f) : dim(d), format(f) {}
   virtual ~ITexture() {}
   
   virtual void prepare( std::shared_ptr<IRenderContext>& ) = 0;
   
   static std::shared_ptr<ITexture> Create( const glm::uvec2&, Format );
   
protected:
   glm::uvec2 dim;
   Format format;
};

class IRenderTarget : public ITexture {
public:
   enum Type {
      Color,
      Depth,
      Stencil
   };
   
   enum Resource {
      FrameBuffer,
      Memory
   };
   
   IRenderTarget( const glm::uvec2& d, Format f, Type t, Resource r ) : ITexture(d, f), type(t), resource(r) {}
   virtual ~IRenderTarget() {}
   
   static std::shared_ptr<IRenderTarget> Create( const glm::uvec2&, Format, Type, Resource );
   
   Resource getResource() { return resource; }
   
protected:
   Type type;
   Resource resource;
   
private:
#if defined ENGINE_BUILD
   template<class Archive> friend inline void boost::serialization::serialize( Archive &, IRenderTarget&, const unsigned int );
   template<class Archive> friend inline void boost::serialization::save_construct_data( Archive &, const IRenderTarget*, const unsigned int );
#endif
};
