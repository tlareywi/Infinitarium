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
   Renderer* _renderer;
   
   id <MTLDevice> _device;
   id <MTLCommandQueue> _commandQueue;
   
   id<CAMetalDrawable> _drawable;
   MTLRenderPassDescriptor* _renderPass;
    
    WKWebView* _uiOverlay;
   
   bool finishedLoading;
}

- (void)loadView {
   finishedLoading = false;
   self.view = (NSView*)[[SimulationView alloc] init];
    _view = (SimulationView*)self.view;
    [_view setAutoresizesSubviews:true];
   CGRect newFrame = CGRectMake( _view.frame.origin.x, _view.frame.origin.y, 1920, 1080);
   _view.frame = newFrame;
    
    WKWebViewConfiguration* config = [WKWebViewConfiguration alloc];
    _uiOverlay = [[WKWebView alloc] initWithFrame:newFrame configuration:config];
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
   
   _renderLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
   _renderLayer.device = _device;
   _renderLayer.framebufferOnly = YES;
   _renderLayer.frame = _view.layer.frame;
   _renderLayer.displaySyncEnabled = NO;
   _renderLayer.drawableSize = _view.frame.size;
   
   [_view addSubview:_uiOverlay];
    
    NSURL* url = [NSURL fileURLWithPath:@"/Users/tlareywi/Source/Infinitarium/UI/defaultOverlay.html"];
    NSURLRequest* request = [NSURLRequest requestWithURL:url];
    [_uiOverlay loadRequest:request];
   
   _commandQueue = [_device newCommandQueue];
   
   finishedLoading = true;
}

-(id<CAMetalDrawable>) currentDrawable {
   while( !_drawable )
      _drawable = [_renderLayer nextDrawable];
   
   return _drawable;
}

-(MTLRenderPassDescriptor*) currentFramebuffer {
   if( _renderPass )
      return _renderPass;
   
   id<CAMetalDrawable> drawable = [self currentDrawable];
   if( drawable ) {
      _renderPass = [MTLRenderPassDescriptor renderPassDescriptor];
      _renderPass.colorAttachments[0].texture = drawable.texture;
      _renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
      _renderPass.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 1.0, 1.0);
      _renderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
   }
   
   return _renderPass;
}

- (void)drawFrame
{
   /// Per frame updates here
   if( !finishedLoading ) return;
   
   id <MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];
   commandBuffer.label = @"MyCommand";
   
   MTLRenderPassDescriptor* renderPassDescriptor = [self currentFramebuffer];
   
   if(renderPassDescriptor != nil) {
      
      id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
      renderEncoder.label = @"MyRenderEncoder";
      
      [renderEncoder endEncoding];
      [commandBuffer presentDrawable:[self currentDrawable]];
   }
   
   [commandBuffer commit];
   
   _renderPass = nil;
   _drawable = nil;
}

- (void)keyDown:(NSEvent *)theEvent {
   NSLog(@"keyDown Detected");
}

- (void)mouseDown:(NSEvent *)theEvent {
   NSLog(@"mouseDown Detected");
}

- (BOOL)acceptsFirstResponder {
   return YES;
}

@end
