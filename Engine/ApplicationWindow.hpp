//
//  SimulationWindow.hpp
//  InfinitariumEngine
//
//  Created by Trystan Larey-Williams on 11/8/18.
//

#pragma once

#include <memory>

#include "RenderContext.hpp"

class IApplicationWindow {
public:
   virtual ~IApplicationWindow() {}
   IApplicationWindow( const IApplicationWindow& ) {}
   
   static std::shared_ptr<IApplicationWindow> Create();
   
   virtual void init( IRenderContext& ) = 0;
   virtual void* getPlatformWindow() = 0;
   virtual void toggleFullScreen(IRenderContext&) = 0;
   virtual void* getPlatformSurface() = 0;

protected:
   IApplicationWindow() {}
};


