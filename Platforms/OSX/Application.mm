//
//  Simulation.cpp
//  InfinitariumOSX
//
//  Created by Trystan Larey-Williams on 11/10/18.
//

#import <Cocoa/Cocoa.h>

#include "Application.hpp"
#include "ViewController.hpp"

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

OSXApplication::OSXApplication() {
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

std::shared_ptr<IApplication> OSXApplication::instance {nullptr};

std::shared_ptr<IApplication> OSXApplication::Instance() {
   if( !instance )
      instance = std::make_shared<OSXApplication>();
   
   return instance;
}
   
void OSXApplication::run() {
     [[NSApplication sharedApplication] run];
}
   
void OSXApplication::stop() {
      [[NSApplication sharedApplication] stop:nullptr];
}

void OSXApplication::addManipulator( const std::string& name, float min, float max, float step ) {
   NSApplication* app = [NSApplication sharedApplication];
   NSWindow* window = [app windows][0];
   NSString* nameStr = [NSString stringWithUTF8String:name.c_str()];
   GameViewController* controler = (GameViewController*)[window contentViewController];
   [controler addManipulator:nameStr :min :max :step];
}

std::shared_ptr<IApplication> CreateApplication() {
   return OSXApplication::Instance();
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
