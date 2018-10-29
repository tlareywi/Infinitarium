//
//  main.m
//  MetalTest
//
//  Created by Trystan Larey-Williams on 5/28/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#import "GameViewController.h"

void EntryPointOSXMainThread() {
    NSApplication* app = [NSApplication sharedApplication];
    [app setDelegate:[[AppDelegate alloc] init]];
    [app run];
}
