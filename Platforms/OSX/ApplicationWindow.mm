//
//  SimulationView.m
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/2/18.
//

#include "ApplicationWindow.hpp"

#import <Cocoa/Cocoa.h>
#import "ViewController.hpp"

#include "../../Engine/EventSampler.hpp"
#include "../../Engine/RenderContext.hpp"

#include <iostream>
///
/// brief OSX implementation of IEventSampler
///
class OSXEventSampler : public IEventSampler {
public:
   
private:
   
};

static std::shared_ptr<IEventSampler> eventSampler = std::make_shared<OSXEventSampler>();

extern "C" {
   std::shared_ptr<IEventSampler> CreateEventSampler() {
      return eventSampler;
   }
}

@interface BorderlessWindow : NSWindow {} @end

@implementation BorderlessWindow

- (BOOL)acceptsFirstResponder {
   return YES;
}

- (BOOL)canBecomeKeyWindow {
   return YES;
}

- (BOOL)canBecomeMainWindow {
   return YES;
}

- (void)cancelOperation:(id)sender
{
   [self toggleFullScreen:nullptr];
}

- (BOOL)acceptsMouseMovedEvents {
   return YES;
}

- (void)keyDown:(NSEvent *)event {
   NSString* chars = event.characters;
   std::string str( [chars UTF8String] );
   for( auto c : str ) {
      if( c == '`' ) {
         for( NSView* object in _contentView.subviews ) {
            bool hidden = [object isHidden];
            [object setHidden:!hidden];
         }
      }
      
      IEventSampler::Key k;
      k.key = c;
      k.state = IEventSampler::DOWN;
      eventSampler->push( k );
   }
}

- (void)mouseMoved:(NSEvent *)event {
   IEventSampler::MouseMove mm;
   mm.dx = event.deltaX;
   mm.dy = event.deltaY;
   mm.dz = event.deltaZ;
   eventSampler->push( mm );
}

- (void)mouseDragged:(NSEvent *)event {
   IEventSampler::MouseDrag mm;
   mm.dx = event.deltaX;
   mm.dy = event.deltaY;
   eventSampler->push( mm );
}

@end

@interface WindowDelegate : NSObject<NSWindowDelegate> {}
   @property (strong) NSWindow* window;
   @property NSRect origRect;
@end
@implementation WindowDelegate
- (NSSize)window:(NSWindow *)window willUseFullScreenContentSize:(NSSize)proposedSize {
   return proposedSize;
}

- (void)windowWillEnterFullScreen:(NSNotification *)notification {
   _origRect = self.window.frame;
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification {

}

- (void)windowDidExitFullScreen:(NSNotification *)notification {
   [_window setFrame:_origRect display:YES animate:YES];
}

- (void)windowDidResize:(NSNotification *)notification {

}
@end


OSXSimulationWindow::OSXSimulationWindow( IRenderContext& context ) {
   GameViewController* controller = [[GameViewController alloc] init];
   
   controller.rect = CGRectMake( context.x(), context.y(), context.width(), context.height() );
   
   controller.backingLayer = (CALayer*)context.getSurface();
   NSWindow* window = [BorderlessWindow windowWithContentViewController:controller];
   
   WindowDelegate* winDelegate = [[WindowDelegate new] init];
   window.delegate = winDelegate;
   winDelegate.window = window;
   
   [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];

   [window setStyleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable)];
   
   [window setTitle:@"Infinitarium"];
   
   if( context.fullScreen() )
      [window toggleFullScreen:nullptr];
}

extern "C" {
   std::shared_ptr<IApplicationWindow> CreateApplicationWindow( IRenderContext& context ) {
      std::shared_ptr<IApplicationWindow> window = std::make_shared<OSXSimulationWindow>( context );
      return window;
   }
}


