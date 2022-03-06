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
   ITexture( const ITexture& obj ) : dim(obj.dim), format(obj.format), image(obj.image), _objId(obj._objId), name(obj.name) {}
   virtual ~ITexture() {}
   
   virtual void prepare( IRenderContext& ) = 0;
   
   static std::shared_ptr<ITexture> Create( unsigned int, unsigned int, Format );
   static std::shared_ptr<ITexture> Clone( const ITexture& );
   static void clearRegisteredObjs();
   
   void set( DataPackContainer& i ) {
         image = std::move(i);
   }

   void setName( const std::string& n ) {
       name = n;
   }

   void setExtent(unsigned int width, unsigned int height) {
       dim.x = width;
       dim.y = height;
   }

   glm::uvec2 getExtent() {
        return dim;
   }
   
protected:
   ITexture() : _objId(reinterpret_cast<unsigned long long>(this)) {};
   glm::uvec2 dim;
   Format format;
   DataPackContainer image;

   unsigned long long _objId;
   std::string name;
};

class TextureProxy : public ITexture {
public:
    TextureProxy() {}
    TextureProxy(const ITexture& obj) : ITexture(obj) {}

    void prepare(IRenderContext&) override {};

    template<class Archive> void load(Archive& ar);
    template<class Archive> void save(Archive& ar) const;

private:
#if defined ENGINE_BUILD
    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive&, unsigned int);
#endif
};

// TODO: We can get rid of these ifdefs by moving proxy classes to a separate header.
#if defined ENGINE_BUILD
	#include <boost/serialization/export.hpp>
   BOOST_SERIALIZATION_ASSUME_ABSTRACT(ITexture)
   BOOST_CLASS_EXPORT_KEY(BlendState)
   BOOST_CLASS_EXPORT_KEY(TextureProxy)
#endif
