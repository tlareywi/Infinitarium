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
   ITexture( const ITexture& obj ) : dim(obj.dim), format(obj.format) {}
   
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
   ITexture() {};
   glm::uvec2 dim;
   Format format;
};

class IRenderTarget : public ITexture {
public:
   IRenderTarget( const IRenderTarget& obj ) :  ITexture(obj), type(obj.type), resource(obj.resource), clear(obj.clear), clearColor(obj.clearColor) {}
   
   enum Type {
      Color,
      Depth,
      Stencil
   };
   
   enum Resource {
      FrameBuffer,
      Memory
   };
   
   virtual ~IRenderTarget() {}
   
   static std::shared_ptr<IRenderTarget> Create( unsigned int, unsigned int, Format, Type, Resource );
   static std::shared_ptr<IRenderTarget> Clone( const IRenderTarget& );
   
   Resource getResource() { return resource; }
   bool getClear() { return clear; }
   void setClear( bool c ) { clear = c; }
   glm::vec4 getClearColor() { return clearColor; }
   void setClearColor( float r, float g, float b, float a ) { clearColor = glm::vec4(r,g,b,a); }
   
protected:
   IRenderTarget() : clear(false), clearColor(glm::vec4(0.0,0,0.0,1.0)) {};
   IRenderTarget( const glm::uvec2& d, Format f, Type t, Resource r ) : ITexture(d, f), type(t), resource(r), clear(false), clearColor(glm::vec4(0.1,0,0.25,1.0)) {}
   Type type;
   Resource resource;
   bool clear;
   glm::vec4 clearColor;
};

class RenderTargetProxy : public IRenderTarget {
public:
   RenderTargetProxy() {}
   RenderTargetProxy(const IRenderTarget& obj) : IRenderTarget(obj) {}
   
   void prepare( std::shared_ptr<IRenderContext>& ) override {};
   
   template<class Archive> void load( Archive& ar );
   template<class Archive> void save( Archive& ar ) const;
   
private:
#if defined ENGINE_BUILD
   friend class boost::serialization::access;
   template<class Archive> friend void boost::serialization::serialize( Archive &, RenderTargetProxy&, unsigned int );
#endif
};

#if defined ENGINE_BUILD
BOOST_SERIALIZATION_ASSUME_ABSTRACT(ITexture)
BOOST_SERIALIZATION_ASSUME_ABSTRACT(IRenderTarget)
#endif