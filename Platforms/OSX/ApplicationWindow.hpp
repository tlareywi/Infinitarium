//
//  SimulationView.h
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/2/18.
//

#pragma once

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

#include "../../Engine/ApplicationWindow.hpp"

class OSXSimulationWindow : public IApplicationWindow {
public:
   OSXSimulationWindow() : window(nullptr) {}
   OSXSimulationWindow( const IApplicationWindow& obj ) : IApplicationWindow( obj ) {};
   virtual ~OSXSimulationWindow();
   
   void init( IRenderContext& ) override;
   
private:
   NSWindow* window;
};
