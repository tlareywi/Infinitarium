//
//  main.m
//  MetalTest
//
//  Created by Trystan Larey-Williams on 5/28/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "GameViewController.h"

#include "EngineImpl.hpp"

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
      IEventSampler::Instance()->push( k );
   }
}

- (void)mouseMoved:(NSEvent *)event {
   IEventSampler::MouseMove mm;
   mm.dx = event.deltaX;
   mm.dy = event.deltaY;
   mm.dz = event.deltaZ;
   IEventSampler::Instance()->push( mm );
}

- (void)mouseDragged:(NSEvent *)event {
   IEventSampler::MouseDrag mm;
   mm.dx = event.deltaX;
   mm.dy = event.deltaY;
   IEventSampler::Instance()->push( mm );
}

@end

void EntryPointOSXMainThread() {
   NSApplication* app = [NSApplication sharedApplication];
   NSViewController* controller = [[GameViewController alloc] init];
   NSWindow* window = [BorderlessWindow windowWithContentViewController:controller];
   
   id menubar    = [NSMenu new];
   id appMenuItem = [NSMenuItem new];
   [menubar addItem:appMenuItem];
   [NSApp setMainMenu:menubar];
   id appMenu      = [NSMenu new];
   id appName      = [[NSProcessInfo processInfo] processName];
   id quitTitle   = [@"Quit " stringByAppendingString:appName];
   id quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle action:@selector(terminate:) keyEquivalent:@"q"];
   [appMenu addItem:quitMenuItem];
   [appMenuItem setSubmenu:appMenu];
   
   [window setTitle:@"Infinitarium"];
   [window makeKeyAndOrderFront:nullptr];
   [app activateIgnoringOtherApps:YES];
   
   [app run];
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


