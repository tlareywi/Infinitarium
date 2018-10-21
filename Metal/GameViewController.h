//
//  GameViewController.h
//  MetalTest
//
//  Created by Trystan Larey-Williams on 5/28/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <Webkit/WKUIDelegate.h>
#import "Renderer.h"

// Our macOS view controller.
@interface GameViewController : NSViewController
- (id<MTLDevice>)getDevice;
- (CAMetalLayer*)getLayer;
@end
