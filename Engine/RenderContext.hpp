//
//  RenderContext.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#pragma once

#include <memory>

class IRenderContext {
public:
   IRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h) : _x(x), _y(y), _width(w), _height(h) {
      
   }
   static std::shared_ptr<IRenderContext> Create( unsigned int, unsigned int, unsigned int, unsigned int );
   
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
   
private:
   unsigned int _x;
   unsigned int _y;
   unsigned int _width;
   unsigned int _height;
};
