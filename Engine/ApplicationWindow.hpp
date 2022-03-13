//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
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


