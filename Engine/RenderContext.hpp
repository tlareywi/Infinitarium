//
//  RenderContext.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#pragma once

#include <memory>
#include <iostream>

#include "../config.h"

#if defined ENGINE_BUILD
   #include <boost/archive/polymorphic_xml_woarchive.hpp>
   #include <boost/archive/polymorphic_xml_wiarchive.hpp>
   #include <boost/serialization/export.hpp>
#endif

class IRenderContext {
public:
   IRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs ) : _x(x), _y(y), _width(w), _height(h), _fullScreen(fs) {
      
   }
   IRenderContext( const IRenderContext& obj ) : _x(obj._x), _y(obj._y), _width(obj._width), _height(obj._height),
   _fullScreen(obj._fullScreen) {}
   virtual ~IRenderContext() {
      std::cout<<"RenderContext destructor"<<std::endl;
   }
   
   static std::shared_ptr<IRenderContext> Create( unsigned int, unsigned int, unsigned int, unsigned int, bool );
   static std::shared_ptr<IRenderContext> Clone( const IRenderContext& );
 
   #if defined ENGINE_BUILD
   void init();
   #endif
   
   virtual void* getSurface() = 0; 
   virtual void setSurface( void*, void* ) = 0;
   virtual void pauseRendering(bool) = 0;
   
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

   void setContextExtent( unsigned int w, unsigned int h ) {
       _width = w;
       _height = h;
       std::cout << "Using fullscreen window mode @" << w << "x" << h << std::endl;
   }
   
protected:
   IRenderContext() : _x(0), _y(0), _width(0), _height(0), _fullScreen(false) {}
   
   unsigned int _x;
   unsigned int _y;
   unsigned int _width;
   unsigned int _height;
   bool _fullScreen;
};

class RenderContextProxy : public IRenderContext {
public:
   RenderContextProxy() {}
   RenderContextProxy( const IRenderContext& obj ) : IRenderContext(obj) {}
   
   void* getSurface() override { return nullptr; };
   void setSurface(void*, void*) override {};
   void pauseRendering(bool) override {};
  
private:
#if defined ENGINE_BUILD
   friend class boost::serialization::access;
   template<class Archive> void serialize( Archive &, const unsigned int );
#endif
};

#if defined ENGINE_BUILD
BOOST_CLASS_EXPORT_KEY(RenderContextProxy);
#endif

