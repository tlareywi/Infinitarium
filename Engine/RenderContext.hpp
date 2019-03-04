//
//  RenderContext.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#pragma once

#include <memory>

#if defined ENGINE_BUILD
   #include <boost/archive/binary_oarchive.hpp>
   #include <boost/archive/binary_iarchive.hpp>
#endif

class IRenderContext {
public:
   IRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs ) : _x(x), _y(y), _width(w), _height(h), _fullScreen(fs) {
      
   }
   virtual ~IRenderContext() {}
   
   static std::shared_ptr<IRenderContext> Create( unsigned int, unsigned int, unsigned int, unsigned int, bool );
   
   virtual void* getSurface() = 0;
   
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
   
private:
   unsigned int _x;
   unsigned int _y;
   unsigned int _width;
   unsigned int _height;
   bool _fullScreen;
};

