//
//  GameViewController.h
//  MetalTest
//
//  Created by Trystan Larey-Williams on 5/28/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "../../Engine/RenderContext.hpp"

// Our macOS view controller.
@interface GameViewController : NSViewController

@property (strong) CALayer* backingLayer;
@property CGRect rect;

@end


