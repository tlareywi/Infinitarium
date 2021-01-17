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

class IApplicationWindow;
class IDataBuffer;

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
   virtual void toggleFullScreen() = 0;
   virtual void resizePickBuffer() = 0;

   // OpenXR integration ///
   virtual unsigned int getPerspectiveCount() { return 1; }
   virtual void getPerspective(unsigned int, glm::dmat4& proj, glm::dmat4& view) {
       proj = glm::dmat4(1.0);
       view = glm::dmat4(1.0);
   }
   /////////////////////////

   void unInit() {
       _pickBuffer = nullptr;
   }
   
   unsigned int x() noexcept {
      return _x;
   }
   
   unsigned int y() noexcept {
      return _y;
   }
   
   unsigned int width() noexcept {
      return _width;
   }
   
   unsigned int height() noexcept {
      return _height;
   }
   
   bool fullScreen() noexcept {
      return _fullScreen;
   }

   std::shared_ptr<IDataBuffer> pickBuffer() noexcept {
       return _pickBuffer;
   }

   bool headset() noexcept {
       return _headset;
   }

   void setContextExtent( unsigned int w, unsigned int h ) noexcept {
       _width = w;
       _height = h;
       resizePickBuffer();
   }
   
protected:
   IRenderContext() : _x(0), _y(0), _width(0), _height(0), _fullScreen(false), _headset(false), _objId(0) {}
   std::shared_ptr<IApplicationWindow> window{ nullptr };
   std::shared_ptr<IDataBuffer> _pickBuffer{ nullptr };
   
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
   void toggleFullScreen() override {};
   void resizePickBuffer() override {};
  
private:
#if defined ENGINE_BUILD
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive &, const unsigned int );
#endif
};

#if defined ENGINE_BUILD
BOOST_CLASS_EXPORT_KEY(RenderContextProxy);
#endif

