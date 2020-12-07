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
#include "DataBuffer.hpp"
#include "../config.h"

struct BlendState {
   enum Op {
      Add,
      Subtract,
      ReverseSubtract,
      Min,
      Max
   };
   enum Factor {
      Zero,
      One,
      SourceColor,
      OneMinusSourceColor,
      SourceAlpha,
      OneMinusSourceAlpha,
      DestinationColor,
      OneMinusDestinationColor,
      DestinationAlpha,
      OneMinusDestinationAlpha,
      SourceAlphaSaturated,
      BlendColor,
      OneMinusBlendColor,
      BlendAlpha,
      OneMinusBlendAlpha,
      Source1Color,
      OneMinusSource1Color,
      Source1Alpha,
      OneMinusSource1Alpha
   };
   
   BlendState() :
      enabled(false),
      rgbBlendOperation( Op::Add ),
      alphaBlendOperation( Op::Add ),
      sourceRGB( Factor::Zero ),
      sourceAlpha( Factor::Zero ),
      destinationRGB( Factor::Zero ),
      destinationAlpha( Factor::Zero ) {
   }
   
   BlendState( Op rgb, Op alpha, Factor srcrgb, Factor dstrgb, Factor srcalpha, Factor dstalpha ) :
      enabled(true),
      rgbBlendOperation(rgb),
      alphaBlendOperation(alpha),
      sourceRGB(srcrgb),
      sourceAlpha(srcalpha),
      destinationRGB(dstrgb),
      destinationAlpha(dstalpha) {
   }
   
   BlendState& operator =( const BlendState& s ) {
      enabled = s.enabled;
      rgbBlendOperation = s.rgbBlendOperation;
      alphaBlendOperation = s.alphaBlendOperation;
      sourceRGB = s.sourceRGB;
      sourceAlpha = s.sourceAlpha;
      destinationRGB = s.destinationRGB;
      destinationAlpha = s.destinationAlpha;
      
      return *this;
   }
   
   bool enabled;
   Op rgbBlendOperation;
   Op alphaBlendOperation;
   Factor sourceRGB;
   Factor sourceAlpha;
   Factor destinationRGB;
   Factor destinationAlpha;
   
private:
#if defined ENGINE_BUILD
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive &, unsigned int );
#endif
};

class ITexture {
public:
   enum Format {
      BRGA8,
      BRGA8_sRGB,
      RGBA8_sRGB,
      RU32,
      RGBA8,
      RF32
   };

   ITexture( const glm::uvec2& d, Format f) : dim(d), format(f), _objId(reinterpret_cast<unsigned long long>(this)) {}
   ITexture( const ITexture& obj ) : dim(obj.dim), format(obj.format), image(obj.image), _objId(obj._objId) {}
   virtual ~ITexture() {}
   
   virtual void prepare( IRenderContext& ) = 0;
   
   static std::shared_ptr<ITexture> Create( unsigned int, unsigned int, Format );
   static std::shared_ptr<ITexture> Clone( const ITexture& );
   
   void set( DataPackContainer& i ) {
         image = std::move(i);
   }

   void setExtent(unsigned int width, unsigned int height) {
       dim.x = width;
       dim.y = height;
   }
   
protected:
   ITexture() : _objId(reinterpret_cast<unsigned long long>(this)) {};
   glm::uvec2 dim;
   Format format;
   DataPackContainer image;

   unsigned long long _objId;
};

class IRenderTarget : public ITexture {
public:
   IRenderTarget( const IRenderTarget& obj ) :  ITexture(obj), type(obj.type), resource(obj.resource), clear(obj.clear), clearColor(obj.clearColor), blending(obj.blending) {}
   
   enum Type {
      Color,
      Depth,
      Stencil
   };
   
   enum Resource {
      Swapchain,
      Offscreen
   };
   
   virtual ~IRenderTarget() {}
   
   static std::shared_ptr<IRenderTarget> Create( unsigned int, unsigned int, Format, Type, Resource );
   static std::shared_ptr<IRenderTarget> Clone( const IRenderTarget& );
   
   Resource getResource() { return resource; }
   bool getClear() { return clear; }
   void setClear( bool c ) { clear = c; }
   glm::vec4 getClearColor() { return clearColor; }
   void setClearColor( float r, float g, float b, float a ) { clearColor = glm::vec4(r,g,b,a); }
   const BlendState& getBlendState() const {
      return blending;
   }
   void setBlendState( const BlendState& blendState ) {
      blending = blendState;
   }
   virtual void getData( const glm::uvec4&, void* ) = 0;
   
protected:
   IRenderTarget() : ITexture(), clear(false), clearColor(glm::vec4(0.0,0,0.0,1.0)) {};
   IRenderTarget( const glm::uvec2& d, Format f, Type t, Resource r ) : ITexture(d, f), type(t), resource(r), clear(false), clearColor(glm::vec4(0.1,0,0.25,1.0)) {}
   Type type;
   Resource resource;
   bool clear;
   glm::vec4 clearColor;
   BlendState blending;
};

//
// Proxy Classes for serialization ///////////////////////////////////////////////////////////////////////////
//

// TODO: Move to separate header to cleanup if defs

class RenderTargetProxy : public IRenderTarget {
public:
   RenderTargetProxy() {}
   RenderTargetProxy(const IRenderTarget& obj) : IRenderTarget(obj) {}
   
   void prepare( IRenderContext& ) override {};
   void getData( const glm::uvec4&, void* ) override {};
   
   template<class Archive> void load( Archive& ar );
   template<class Archive> void save( Archive& ar ) const;
   
private:
#if defined ENGINE_BUILD
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive &,  unsigned int );
#endif
};

class TextureProxy : public ITexture {
public:
   TextureProxy() {}
   TextureProxy(const ITexture& obj) : ITexture(obj) {}
   
   void prepare( IRenderContext& ) override {};
   
   template<class Archive> void load( Archive& ar );
   template<class Archive> void save( Archive& ar ) const;
   
private:
#if defined ENGINE_BUILD
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive &, unsigned int );
#endif
};

#if defined ENGINE_BUILD
	#include <boost/serialization/export.hpp>
   BOOST_SERIALIZATION_ASSUME_ABSTRACT(ITexture)
   BOOST_SERIALIZATION_ASSUME_ABSTRACT(IRenderTarget)
   BOOST_CLASS_EXPORT_KEY(BlendState)
   BOOST_CLASS_EXPORT_KEY(RenderTargetProxy)
   BOOST_CLASS_EXPORT_KEY(TextureProxy)
#endif
