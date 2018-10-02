//
//  SimulationView.h
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/2/18.
//

#pragma once

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

@interface SimulationView : NSView
   @property CAMetalLayer* metalLayer;
@end
