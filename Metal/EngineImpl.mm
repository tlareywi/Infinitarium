//
//  DataBuffer.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/13/18.
//

#include "EngineImpl.hpp"

#import <Metal/Metal.h>
#import <Cocoa/Cocoa.h>

#import "AppDelegate.h"
#include "GameViewController.h"

// TODO: Refector to runtime loaded library

class MetalDataBuffer : public IDataBuffer {
public:
   void set( DataPackContainer& ) override;
   
private:
   id<MTLBuffer> buffer;
};

std::shared_ptr<IDataBuffer> CreateMetalDataBuffer() {
   return std::make_shared<MetalDataBuffer>();
}

void MetalDataBuffer::set( DataPackContainer& datapack ) {
   NSViewController* ctrl = [[[NSApplication sharedApplication] mainWindow] contentViewController];
   if( !ctrl )
      return;
      
   id<MTLDevice> device = [(GameViewController*)ctrl getDevice];
   
   unsigned int size = 0;
   std::visit( [&size](auto const& e){ size = e.sizeBytes(); }, datapack );
      
   buffer = [device newBufferWithLength:size options:MTLResourceStorageModeManaged];
}


