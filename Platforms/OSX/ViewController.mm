//
//  GameViewController.m
//  MetalTest
//
//  Created by Trystan Larey-Williams on 5/28/18.
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

#include "ViewController.hpp"
#include "ApplicationWindow.hpp"
#include "Application.hpp"

#import <Webkit/WKWebView.h>
#import <Webkit/WKWebViewConfiguration.h>
#import <Webkit/WKNavigationDelegate.h>
#import <WebKit/WKUserContentController.h>
#import <WebKit/WKScriptMessage.h>

#include <iostream>

@interface WebNavDelegate : NSObject<WKNavigationDelegate> {}
@end
@implementation WebNavDelegate
- (void)webView:(WKWebView *)webView didFailNavigation:(WKNavigation *)navigation withError:(NSError *)error {
   std::cout<<[[error localizedDescription] UTF8String]<<std::endl;
}

- (void)webView:(WKWebView *)webView didFailProvisionalNavigation:(WKNavigation *)navigation withError:(NSError *)error {
   std::cout<<[[error localizedDescription] UTF8String]<<std::endl;
}
@end

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
   [_view setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
   CGRect newFrame = CGRectMake( self.rect.origin.x, self.rect.origin.x, self.rect.size.width, self.rect.size.height );
   _view.frame = newFrame;
   
   NSURLCache* cache = [NSURLCache sharedURLCache];
   [cache removeAllCachedResponses];
   
   WKWebViewConfiguration* config = [[WKWebViewConfiguration alloc] init];
   
   WKUserContentController* controller = [[WKUserContentController alloc] init];
   [controller addScriptMessageHandler:self name:@"command"];
   [controller addScriptMessageHandler:self name:@"manipulate"];
   config.userContentController = controller;
   
   CGRect consoleFrame = newFrame;
   consoleFrame.origin.y = 0;
   consoleFrame.size.height =  newFrame.size.height / 3.0;
   
   _uiOverlay = [[WKWebView alloc] initWithFrame:consoleFrame configuration:config];
   _uiOverlay.navigationDelegate = [[WebNavDelegate alloc] init];
   [_uiOverlay setAutoresizingMask:NSViewWidthSizable];
   [_uiOverlay setValue:@NO forKey:@"drawsBackground"];
}

- (void)viewDidLoad {
   [super viewDidLoad];
   
   [_view addSubview:_uiOverlay];
   
   NSURL* url = [NSURL fileURLWithPath:@"/usr/local/share/Infinitarium/UI/defaultOverlay.html"];
   NSURLRequest* request = [NSURLRequest requestWithURL:url];
   [_uiOverlay loadRequest:request];
}

- (void)addManipulator :(NSString*)name :(float)min :(float)max :(float)step {
   NSMutableString* js = [[NSMutableString alloc] init];
   [js appendString:@"Manipulator( {id:'"];
   [js appendString:name];
   [js appendString:@"', min:"];
   [js appendString:[NSString stringWithFormat:@"%f", min]];
   [js appendString:@", max:"];
   [js appendString:[NSString stringWithFormat:@"%f", max]];
   [js appendString:@", step:"];
   [js appendString:[NSString stringWithFormat:@"%f", step]];
   [js appendString:@"} )"];
   [_uiOverlay evaluateJavaScript:js completionHandler:^(NSString *result, NSError *error) {
      if( error ) NSLog(@"%@",error);
   }];
}

- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message {
   if( [message.name compare:@"command"] == NSOrderedSame ) {
      std::string cmd {[message.body UTF8String]};
      std::string result = OSXApplication::Instance()->getPythonInterpreter()->eval( cmd );
      
      if( result.empty() )
         return;
      
      std::string js("con.log(\"");
      js += result;
      js += "\").classList.add(\"result\");";
      
      NSString* toJS = [NSString stringWithUTF8String:js.c_str()];
      [_uiOverlay evaluateJavaScript:toJS completionHandler:nil];
   }
   else if( [message.name compare:@"manipulate"] == NSOrderedSame ) {
      std::string cmd {[message.body UTF8String]};
      OSXApplication::Instance()->invoke( cmd );
   }
}

@end
