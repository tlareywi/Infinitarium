//
//  GameViewController.h
//  MetalTest
//
//  Created by Trystan Larey-Williams on 5/28/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <WebKit/WKScriptMessageHandler.h>

#include "../../Engine/RenderContext.hpp"

// Our macOS view controller.
@interface GameViewController : NSViewController<WKScriptMessageHandler>

@property (strong) CALayer* backingLayer;
@property CGRect rect;

- (void)addManipulator: (NSString*)name :(float)min :(float)max :(float)step;

@end


