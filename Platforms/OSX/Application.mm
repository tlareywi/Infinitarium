//
//  Simulation.cpp
//  InfinitariumOSX
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#import <Cocoa/Cocoa.h>

#include "../../Engine/Application.hpp"

#include <iostream>

@interface AppDelegate : NSObject<NSApplicationDelegate>

@end


@implementation AppDelegate
-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
   return YES;
}

-(void)applicationDidFinishLaunching:(NSNotification *)notification {
   NSApplication* app = [NSApplication sharedApplication];

   NSWindow* window = [app windows][0];

   [window makeKeyAndOrderFront:nil];
   [window makeFirstResponder:nil];
   [window setLevel:NSNormalWindowLevel];
   [app activateIgnoringOtherApps:YES];
}
@end

// TODO: This probably needs to be a singleton. Enforce at engine level?
class OSXApplication : public IApplication {
public:
   OSXApplication() {
      NSApplication* app = [NSApplication sharedApplication];
      [app setActivationPolicy:NSApplicationActivationPolicyRegular];
      [app setDelegate: [AppDelegate new]];
      
      id menubar    = [NSMenu new];
      id appMenuItem = [NSMenuItem new];
      [menubar addItem:appMenuItem];
      [app setMainMenu:menubar];
      id appMenu      = [NSMenu new];
      id appName      = [[NSProcessInfo processInfo] processName];
      id quitTitle   = [@"Quit " stringByAppendingString:appName];
      id quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle action:@selector(terminate:) keyEquivalent:@"q"];
      [appMenu addItem:quitMenuItem];
      [appMenuItem setSubmenu:appMenu];
      
      [app setPresentationOptions:NSApplicationPresentationAutoHideDock|NSApplicationPresentationAutoHideMenuBar|NSApplicationPresentationFullScreen];
   }
   
   void run() override {
     [[NSApplication sharedApplication] run];
   }
   
   void stop() override {
      [[NSApplication sharedApplication] stop:nullptr];
   }

};

extern "C" {
   std::shared_ptr<IApplication> CreateApplication() {
      std::shared_ptr<IApplication> app = std::make_shared<OSXApplication>();
      return app;
   }
}

//
// May want to use this in the future instead of built-in NSApplication run
//

/* - (void)run
 {
 NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
 
 [self finishLaunching];
 
 shouldKeepRunning = YES;
 do
 {
 [pool release];
 pool = [[NSAutoreleasePool alloc] init];
 
 NSEvent *event =
 [self
 nextEventMatchingMask:NSAnyEventMask
 untilDate:[NSDate distantFuture]
 inMode:NSDefaultRunLoopMode
 dequeue:YES];
 
 [self sendEvent:event];
 [self updateWindows];
 } while (shouldKeepRunning);
 
 [pool release];
 }
 
 - (void)terminate:(id)sender
 {
 shouldKeepRunning = NO;
 } */
