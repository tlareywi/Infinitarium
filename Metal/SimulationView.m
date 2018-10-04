//
//  SimulationView.m
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/2/18.
//

#import "SimulationView.h"

@implementation SimulationView {
   
}

+ (Class) layerClass {
   return [CAMetalLayer class];
}

- (id) metalLayer {
    if( !self.layer )
        self.layer = [[CAMetalLayer alloc] init];
    return (CAMetalLayer*)self.layer;
}

@end

