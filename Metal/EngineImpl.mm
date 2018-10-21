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
   
   unsigned long size = 0;
   std::visit( [&size](auto const& e){ size = e.sizeBytes(); }, datapack );
      
   buffer = [device newBufferWithLength:size options:MTLResourceStorageModeManaged];
}

class MetalRenderPass : public IRenderPass {
public:
   void begin() override {
      NSViewController* ctrl = [[[NSApplication sharedApplication] mainWindow] contentViewController];
      if( !ctrl ) return;
      
      if( commandQ == nullptr ) { // TODO: refactor
         id<MTLDevice> device = [(GameViewController*)ctrl getDevice];
         commandQ = [device newCommandQueue];
      }
      
      commandBuffer = [commandQ commandBuffer];
      commandBuffer.label = @"IRenderPass";
      
      CAMetalLayer* layer = [(GameViewController*)ctrl getLayer];
      
      while( !drawable )
         drawable = [layer nextDrawable];
      
      renderPass = [MTLRenderPassDescriptor renderPassDescriptor];
      renderPass.colorAttachments[0].texture = drawable.texture;
      renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
      renderPass.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 1.0, 1.0);
      renderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
      
      
   }
   
   void end() override {
      [commandBuffer presentDrawable:drawable];
      [commandBuffer commit];
      drawable = nil;
      renderPass = nil;
   }
   
   id <MTLCommandBuffer> getCommandBuffer() {
      return commandBuffer;
   }
   
   MTLRenderPassDescriptor* getPassDescriptor() {
      return renderPass;
   }
   
private:
   static id<MTLCommandQueue> commandQ; // TODO: refactor
   MTLRenderPassDescriptor* renderPass;
   id<CAMetalDrawable> drawable;
   id <MTLCommandBuffer> commandBuffer;
};

id<MTLCommandQueue> MetalRenderPass::commandQ = nullptr;

std::shared_ptr<IRenderPass> CreateMetalRenderPass() {
   return std::make_shared<MetalRenderPass>();
}

class MetalPipelineState : public IPipelineState {
public:
   
private:
   
};

std::shared_ptr<IPipelineState> CreateMetalPipelineState() {
   return std::make_shared<MetalPipelineState>();
}

class MetalRenderCommand : public IRenderCommand {
public:
   void encode( IRenderPass& renderPass ) override {
      MetalRenderPass* metalRenderPass = dynamic_cast<MetalRenderPass*>( &renderPass );
      id <MTLRenderCommandEncoder> renderEncoder = [metalRenderPass->getCommandBuffer() renderCommandEncoderWithDescriptor:metalRenderPass->getPassDescriptor()];
      renderEncoder.label = @"MyRenderEncoder";
      [renderEncoder endEncoding];
   }
private:
   
};

std::shared_ptr<IRenderCommand> CreateMetalRenderCommand() {
   return std::make_shared<MetalRenderCommand>();
}






