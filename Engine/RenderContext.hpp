//
//  RenderContext.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#pragma once

#include <memory>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "../config.h"

#if defined ENGINE_BUILD
#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#include <boost/serialization/export.hpp>
#endif

class IRenderContext {
public:
   IRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs, bool headset ) : 
       _x(x), _y(y), _width(w), _height(h), _fullScreen(fs), _headset(headset), _objId(reinterpret_cast<unsigned long long>(this)) {
   }

   IRenderContext( const IRenderContext& obj ) : _x(obj._x), _y(obj._y), _width(obj._width), _height(obj._height),
   _fullScreen(obj._fullScreen), _headset(obj._headset), _objId(obj._objId) {}
   
   virtual ~IRenderContext() {
   }
   
   static std::shared_ptr<IRenderContext> Create( unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fullScreen, bool headset );
   static std::shared_ptr<IRenderContext> Clone( const IRenderContext& );
   static void clearRegisteredObjs();
 
   #if defined ENGINE_BUILD
   void init();
   #endif
   
   virtual void* getSurface() = 0; 
   virtual void setSurface( void* ) = 0;
   virtual void pauseRendering(bool) = 0;
   virtual void beginFrame() = 0;
   virtual void endFrame() = 0;
   virtual void waitOnIdle() = 0;

   // OpenXR integration ///
   virtual unsigned int getPerspectiveCount() { return 1; }
   virtual void getPerspective(unsigned int, glm::mat4x4& proj, glm::mat4x4& view) { 
       proj = glm::mat4x4(1.0);
       view = glm::mat4x4(1.0); 
   }
   /////////////////////////
   
   unsigned int x() {
      return _x;
   }
   
   unsigned int y() {
      return _y;
   }
   
   unsigned int width() {
      return _width;
   }
   
   unsigned int height() {
      return _height;
   }
   
   bool fullScreen() {
      return _fullScreen;
   }

   bool headset() {
       return _headset;
   }

   void setContextExtent( unsigned int w, unsigned int h ) {
       _width = w;
       _height = h;
       std::cout << "Using fullscreen window mode @" << w << "x" << h << std::endl;
   }
   
protected:
   IRenderContext() : _x(0), _y(0), _width(0), _height(0), _fullScreen(false), _headset(false), _objId(0) {}
   
   unsigned int _x;
   unsigned int _y;
   unsigned int _width;
   unsigned int _height;
   bool _fullScreen;
   bool _headset;
   unsigned long long _objId;
};

class RenderContextProxy : public IRenderContext {
public:
   RenderContextProxy() {}
   RenderContextProxy( const IRenderContext& obj ) : IRenderContext(obj) {}
   
   void* getSurface() override { return nullptr; };
   void setSurface(void*) override {};
   void pauseRendering(bool) override {};
   void beginFrame() override {};
   void endFrame() override {};
   void waitOnIdle() override {};
  
private:
#if defined ENGINE_BUILD
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive &, const unsigned int );
#endif
};

#if defined ENGINE_BUILD
BOOST_CLASS_EXPORT_KEY(RenderContextProxy);
#endif

