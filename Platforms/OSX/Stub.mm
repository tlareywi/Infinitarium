//
//  main.m
//  MetalTest
//
//  Created by Trystan Larey-Williams on 5/28/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "GameViewController.hpp"

#include "../../Engine/EventSampler.hpp"

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




