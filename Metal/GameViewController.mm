//
//  GameViewController.m
//  MetalTest
//
//  Created by Trystan Larey-Williams on 5/28/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

#import "GameViewController.h"
#import "Renderer.h"
#import "SimulationView.h"

#import <simd/simd.h>
#import <ModelIO/ModelIO.h>

// Include header shared between C code here, which executes Metal API commands, and .metal files
#import "ShaderTypes.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Webkit/WKWebView.h>
#import <Webkit/WKWebViewConfiguration.h>

@implementation GameViewController
{
   SimulationView* _view;
   CAMetalLayer* _renderLayer;
   
   id <MTLDevice> _device;
   id <MTLCommandQueue> _commandQ;
    
   WKWebView* _uiOverlay;
}

-(id<MTLDevice>) getDevice {
   return _device;
}

-(id<MTLCommandQueue>) getCommandQ {
   return _commandQ;
}

-(CAMetalLayer*) getLayer {
   return _renderLayer;
}

- (void)loadView {
   self.view = (NSView*)[[SimulationView alloc] init];
    _view = (SimulationView*)self.view;
    [_view setAutoresizesSubviews:true];
   CGRect newFrame = CGRectMake( _view.frame.origin.x, _view.frame.origin.y, 1920, 1080);
   _view.frame = newFrame;
    
    WKWebViewConfiguration* config = [WKWebViewConfiguration alloc];
    CGRect consoleFrame = newFrame;
    consoleFrame.origin.y = 0;
    consoleFrame.size.height =  1080 / 3.0;
    _uiOverlay = [[WKWebView alloc] initWithFrame:consoleFrame configuration:config];
    [_uiOverlay setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
    [_uiOverlay setValue:@NO forKey:@"drawsBackground"];
}

- (void)viewDidLoad {
   [super viewDidLoad];
   
   _renderLayer = _view.metalLayer;
   _device = MTLCreateSystemDefaultDevice();
   
   if(!_device)
   {
      NSLog(@"Metal is not supported on this device");
      self.view = [[NSView alloc] initWithFrame:self.view.frame];
      return;
   }
   
   _commandQ = [_device newCommandQueue];
   
   _renderLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
   _renderLayer.device = _device;
   _renderLayer.framebufferOnly = YES;
   _renderLayer.frame = _view.layer.frame;
   _renderLayer.displaySyncEnabled = NO;
   _renderLayer.drawableSize = _view.frame.size;
   
 //  [_view addSubview:_uiOverlay];
    
 //   NSURL* url = [NSURL fileURLWithPath:@"/Users/trystan/Source/Infinitarium/UI/defaultOverlay.html"];
 //   NSURLRequest* request = [NSURLRequest requestWithURL:url];
  //  [_uiOverlay loadRequest:request];
}

@end
