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

@interface BorderlessWindow : NSWindow {
   enum {
      LEFT = 0,
      RIGHT
   };
   
   std::chrono::time_point<std::chrono::high_resolution_clock> clickTimer;
   std::vector<IEventSampler::MouseButton> mouseButtons;
}

@end

@implementation BorderlessWindow

- (instancetype)initWithContentRect:(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag {
   IEventSampler::MouseButton button;
   button.button = IEventSampler::LEFT;
   mouseButtons.push_back( button );
   
   button.button = IEventSampler::RIGHT;
   mouseButtons.push_back( button );
   
   return [super initWithContentRect:contentRect styleMask:style backing:backingStoreType defer:flag];
}

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

- (void)mouseDown:(NSEvent *)theEvent {
   unsigned short indx{0};
   if( [theEvent type] == NSEventTypeLeftMouseDown )
      indx = LEFT;
   else if( [theEvent type] == NSEventTypeRightMouseDown )
      indx = RIGHT;
   
   NSPoint pos = [theEvent locationInWindow];
   mouseButtons[indx].x = pos.x;
   mouseButtons[indx].y = pos.y;
   
   clickTimer = std::chrono::high_resolution_clock::now();
   mouseButtons[indx].state = IEventSampler::DOWN;
   
   eventSampler->push( mouseButtons[indx] );
}

- (void)mouseUp:(NSEvent *)theEvent {
   unsigned short indx{0};
   if( [theEvent type] == NSEventTypeLeftMouseUp )
      indx = LEFT;
   else if( [theEvent type] == NSEventTypeRightMouseUp )
      indx = RIGHT;
   
   NSPoint pos = [theEvent locationInWindow];
   mouseButtons[indx].x = pos.x;
   mouseButtons[indx].y = pos.y;
   
   if( mouseButtons[indx].state == IEventSampler::DOWN || mouseButtons[indx].state == IEventSampler::CLICKED ) {
      auto clickTime = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> t = clickTime - clickTimer;
      
      if( t.count() < 100 ) {
         if( mouseButtons[indx].state == IEventSampler::DOWN )
            mouseButtons[indx].state = IEventSampler::CLICKED;
         else
            mouseButtons[indx].state = IEventSampler::DBL_CLICKED;
      }
      else
         mouseButtons[indx].state = IEventSampler::UP;
   }
   else
      mouseButtons[indx].state = IEventSampler::UP;
   
   
   eventSampler->push( mouseButtons[indx] );
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

OSXSimulationWindow::~OSXSimulationWindow() {
   [window performClose:window];
}

void OSXSimulationWindow::init( IRenderContext& context ) {
   GameViewController* controller = [[GameViewController alloc] init];
   
   // TODO: Modify to allow multiple contexts (in this case Metal Layers) per window.
   // It's a bit of an 'edge' feature as typically one would just use a viewport, but could
   // allow for some interesting stuff; use of low power GPU for low res preview render?
   controller.rect = CGRectMake( context.x(), context.y(), context.width(), context.height() );
   
   controller.backingLayer = (CALayer*)context.getSurface();
   window = [BorderlessWindow windowWithContentViewController:controller];
   
   WindowDelegate* winDelegate = [[WindowDelegate new] init];
   window.delegate = winDelegate;
   winDelegate.window = window;
   
   [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
   
   [window setStyleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable)];
   
   [window setTitle:@"Infinitarium"];
   
   if( context.fullScreen() )
      [window toggleFullScreen:nullptr];
   
   [window makeKeyAndOrderFront:nil];
   [window makeFirstResponder:nil];
   [window setLevel:NSNormalWindowLevel];
}

extern "C" {
   std::shared_ptr<IApplicationWindow> CreateApplicationWindow() {
      std::shared_ptr<IApplicationWindow> window = std::make_shared<OSXSimulationWindow>();
      return window;
   }
}

extern "C" {
   std::shared_ptr<IApplicationWindow> CloneApplicationWindow( const IApplicationWindow& obj ) {
      std::shared_ptr<IApplicationWindow> window = std::make_shared<OSXSimulationWindow>( obj );
      return window;
   }
}


