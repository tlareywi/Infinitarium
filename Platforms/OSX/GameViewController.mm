//
//  GameViewController.m
//  MetalTest
//
//  Created by Trystan Larey-Williams on 5/28/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

#import "GameViewController.hpp"
#import "ApplicationWindow.hpp"

#import <Webkit/WKWebView.h>
#import <Webkit/WKWebViewConfiguration.h>

@implementation GameViewController
{
   NSView* _view;
   WKWebView* _uiOverlay;
}

- (void)loadView {
   self.view = (NSView*)[[NSView alloc] init];
   _view = self.view;
   _view.layer = self.backingLayer;
   [_view setAutoresizesSubviews:true];
   CGRect newFrame = CGRectMake( self.rect.origin.x, self.rect.origin.x, self.rect.size.width, self.rect.size.height );
   _view.frame = newFrame;
    
   WKWebViewConfiguration* config = [WKWebViewConfiguration alloc];
   CGRect consoleFrame = newFrame;
   consoleFrame.origin.y = 0;
   consoleFrame.size.height =  newFrame.size.height / 3.0;
   _uiOverlay = [[WKWebView alloc] initWithFrame:consoleFrame configuration:config];
   [_uiOverlay setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
   [_uiOverlay setValue:@NO forKey:@"drawsBackground"];
}

- (void)viewDidLoad {
   [super viewDidLoad];
   
 //  [_view addSubview:_uiOverlay];
    
 //   NSURL* url = [NSURL fileURLWithPath:@"/Users/trystan/Source/Infinitarium/UI/defaultOverlay.html"];
 //   NSURLRequest* request = [NSURLRequest requestWithURL:url];
  //  [_uiOverlay loadRequest:request];
}

@end