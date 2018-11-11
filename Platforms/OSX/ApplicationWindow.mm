//
//  SimulationView.m
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/2/18.
//

#include "ApplicationWindow.hpp"

#import <Cocoa/Cocoa.h>
#import "GameViewController.hpp"

#include "../../Engine/EventSampler.hpp"
#include "../../Engine/RenderContext.hpp"

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
- (BOOL)canBecomeKeyWindow {
   return YES;
}

- (BOOL)canBecomeMainWindow {
   return YES;
}

- (BOOL)acceptsMouseMovedEvents {
   return YES;
}

- (void)keyDown:(NSEvent *)event {
   NSString* chars = event.characters;
   std::string str( [chars UTF8String] );
   for( auto c : str ) {
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


OSXSimulationWindow::OSXSimulationWindow( IRenderContext& context ) {
   GameViewController* controller = [[GameViewController alloc] init];
   controller.rect = CGRectMake( context.x(), context.y(), context.width(), context.height() );
   controller.backingLayer = (CALayer*)context.getSurface();
   NSWindow* window = [BorderlessWindow windowWithContentViewController:controller];
   
   [window setTitle:@"Infinitarium"];
   [window makeKeyAndOrderFront:nullptr];
}

extern "C" {
   std::shared_ptr<IApplicationWindow> CreateApplicationWindow( IRenderContext& context ) {
      std::shared_ptr<IApplicationWindow> window = std::make_shared<OSXSimulationWindow>( context );
      return window;
   }
}


