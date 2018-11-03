//
//  DataBuffer.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/13/18.
//

#include "EngineImpl.hpp"

#import <Metal/Metal.h>
#import <Cocoa/Cocoa.h>

#include "GameViewController.h"

#include "ShaderTypes.h"

#include "../Engine/DataBuffer.hpp"
#include "../Engine/RenderState.hpp"
#include "../Engine/RenderPass.hpp"
#include "../Engine/RenderCommand.hpp"
#include "../Engine/RenderProgram.hpp"

// TODO: Refector all this to runtime loaded library

id<MTLDevice> GetMTLDevice() {
   NSViewController* ctrl = [[[NSApplication sharedApplication] mainWindow] contentViewController];
   if( !ctrl )
      return nullptr;
   
   id<MTLDevice> device = [(GameViewController*)ctrl getDevice];
   
   return device;
}

id<MTLCommandQueue> GetMTLCommandQ() {
   NSViewController* ctrl = [[[NSApplication sharedApplication] mainWindow] contentViewController];
   if( !ctrl )
      return nullptr;
   
   id<MTLCommandQueue> cmdQ = [(GameViewController*)ctrl getCommandQ];
   
   return cmdQ;
}

CAMetalLayer* GetMTLLayer() {
   NSViewController* ctrl = [[[NSApplication sharedApplication] mainWindow] contentViewController];
   if( !ctrl )
      return nullptr;
   
   CAMetalLayer* layer = [(GameViewController*)ctrl getLayer];
   
   return layer;
}


///
/// brief Metal implementation of DataBuffer
///
class MetalDataBuffer : public IDataBuffer {
public:
   void set( DataPackContainer& container ) override {
      id<MTLDevice> device = GetMTLDevice();
      if( !device ) {
         std::cout<<"Warning: No device ready for call to IDataBuffer:set(...)."<<std::endl;
         return;
      }
      
      std::visit( [device, this](auto& e) { // TODO: move visit to caller level and pass DataPack instead?
         @autoreleasepool {
            id<MTLBuffer> tmpBuffer = [device newBufferWithBytes:e.get() length:e.sizeBytes() options:MTLResourceStorageModeManaged];
            
            GPU = [device newBufferWithLength:e.sizeBytes() options:MTLResourceStorageModePrivate];
            
            id<MTLCommandBuffer> cmdBuf = [GetMTLCommandQ() commandBuffer];
            id<MTLBlitCommandEncoder> bltEncoder = [cmdBuf blitCommandEncoder];
            [bltEncoder copyFromBuffer:tmpBuffer sourceOffset:0 toBuffer:GPU destinationOffset:0 size:e.sizeBytes()];
            [bltEncoder endEncoding];
            [cmdBuf commit];
         }
      }, container );
   };
   
   void reserve( unsigned int sizeBytes ) override {
      id<MTLDevice> device = GetMTLDevice();
      if( !device ) {
         std::cout<<"Warning: No device ready for call to IDataBuffer:reserve(...)."<<std::endl;
         return;
      }
   
      GPU = [device newBufferWithLength:sizeBytes options:MTLResourceStorageModePrivate];
   }
   
   void set( const void* const data, unsigned int sizeBytes ) override {
      id<MTLDevice> device = GetMTLDevice();
      if( !device ) {
         std::cout<<"Warning: No device ready for call to IDataBuffer:set(...)."<<std::endl;
         return;
      }
      
      @autoreleasepool {
         if( !GPU ) {
            GPU = [device newBufferWithLength:sizeBytes options:MTLResourceStorageModePrivate];
         }
         
         id<MTLBuffer> tmpBuffer = [device newBufferWithBytes:data length:sizeBytes options:MTLResourceStorageModeManaged];
         
         id<MTLCommandBuffer> cmdBuf = [GetMTLCommandQ() commandBuffer];
         id<MTLBlitCommandEncoder> bltEncoder = [cmdBuf blitCommandEncoder];
         [bltEncoder copyFromBuffer:tmpBuffer sourceOffset:0 toBuffer:GPU destinationOffset:0 size:sizeBytes];
         [bltEncoder endEncoding];
         [cmdBuf commit];
      }
   }
   
   id<MTLBuffer> getMTLBuffer() {
      return GPU;
   }
   
private:
   id<MTLBuffer> GPU{nullptr};
};

std::shared_ptr<IDataBuffer> CreateMetalDataBuffer() {
   std::shared_ptr<MetalDataBuffer> databuf = std::make_shared<MetalDataBuffer>();
   return databuf;
}


///
/// brief Metal implementation of RendePass
///
class MetalRenderPass : public IRenderPass {
public:
   void begin() override {
      id<MTLCommandQueue> commandQ = GetMTLCommandQ();
      
      commandBuffer = [commandQ commandBuffer];
      commandBuffer.label = @"IRenderPass";
      
      CAMetalLayer* layer = GetMTLLayer();
      
      drawable = [layer nextDrawable];
      
      renderPass = [MTLRenderPassDescriptor renderPassDescriptor];
      renderPass.colorAttachments[0].texture = drawable.texture;
      renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
      renderPass.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);
      renderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
   }
   
   void end() override {
      if( !drawable ) return;
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
   MTLRenderPassDescriptor* renderPass;
   id<CAMetalDrawable> drawable;
   id <MTLCommandBuffer> commandBuffer;
};
std::shared_ptr<IRenderPass> CreateMetalRenderPass() {
   return std::make_shared<MetalRenderPass>();
}


///
/// brief Metal implementation of RenderState
///
class MetalRenderState : public IRenderState {
public:
   MetalRenderState() {
      renderDescriptor = [MTLRenderPipelineDescriptor new];
      [renderDescriptor reset];
   }
   
   virtual ~MetalRenderState() {
      renderDescriptor = nullptr;
   }
   
   void commit() override {
      NSViewController* ctrl = [[[NSApplication sharedApplication] mainWindow] contentViewController];
      if( !ctrl )
         return;
      
      id<MTLDevice> device = [(GameViewController*)ctrl getDevice];
      
      sanityCheck( device );
      
      MTLRenderPipelineColorAttachmentDescriptor* attachement = renderDescriptor.colorAttachments[0];
      attachement.blendingEnabled = YES;
      attachement.rgbBlendOperation = MTLBlendOperationAdd;
      attachement.alphaBlendOperation = MTLBlendOperationAdd;
      attachement.sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
      attachement.sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
      attachement.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
      attachement.destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
      
      NSError* err {nullptr};
      renderState = [device newRenderPipelineStateWithDescriptor:renderDescriptor error:&err];
      if( err ) {
         std::cout<<"Error creating PipelineState. "<<[err.localizedDescription UTF8String]<<std::endl;
      }
   }
   
   void sanityCheck( id<MTLDevice> device ) { // Absolute minimal pipline specification to not SIGABRT on pipline state creation.
      if( !renderDescriptor.vertexFunction ) {
         id<MTLLibrary> program = [device newDefaultLibrary];
         renderDescriptor.vertexFunction = [program newFunctionWithName:@"vertexShader"];
         renderDescriptor.fragmentFunction = [program newFunctionWithName:@"fragmentShader"];
      }
      
      if( !renderDescriptor.colorAttachments[0].pixelFormat ) {
         renderDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
      }
   }
   
   MTLRenderPipelineDescriptor* getPipelineDescriptor() {
      return renderDescriptor;
   }
   
   id<MTLRenderPipelineState> getPipelineState() const {
      return renderState;
   }
private:
   MTLRenderPipelineDescriptor* renderDescriptor;
   id<MTLRenderPipelineState> renderState;
};
std::shared_ptr<IRenderState> CreateMetalRenderState() {
   return std::make_shared<MetalRenderState>();
}


///
/// brief Metal implementation of RenderProgram
///
class MetalRenderProgram : public IRenderProgram {
   void prepare( IRenderState& state ) override {
      id<MTLDevice> device = GetMTLDevice();
      if( !device ) return;
      
      id<MTLLibrary> program = [device newDefaultLibrary];
      
      vertex = [program newFunctionWithName:@"staticInstancedStarsVert"];
      fragment = [program newFunctionWithName:@"staticInstancedStarsFrag"];
      
      MetalRenderState* metalState = dynamic_cast<MetalRenderState*>( &state );
      metalState->getPipelineDescriptor().vertexFunction = vertex;
      metalState->getPipelineDescriptor().fragmentFunction = fragment;
   }
   
   void apply( IRenderState& state ) override {
   }
   
private:
   id<MTLFunction> vertex;
   id<MTLFunction> fragment;
};
std::shared_ptr<IRenderProgram> CreateMetalRenderProgram() {
   return std::make_shared<MetalRenderProgram>();
}

#include <glm/gtc/matrix_transform.hpp>

template<typename T, typename U> const U static inline convert( const T& t ) {
   U retType;
   memcpy( &retType, &t, sizeof(U) );
   return retType;
}

///
/// brief Metal implementation of RenderCommand
///
class MetalRenderCommand : public IRenderCommand {
public:
   void encode( IRenderPass& renderPass, const IRenderState& state ) override {
      MetalRenderPass* metalRenderPass = dynamic_cast<MetalRenderPass*>( &renderPass );
      const MetalRenderState& metalRenderState = dynamic_cast<const MetalRenderState&>( state );
      if( !metalRenderPass ) {
         std::cout<<"WARNING, failed cast to MetalRenderCommand."<<std::endl;
         return;
      }
      
      MTLRenderPassDescriptor* desc = metalRenderPass->getPassDescriptor();
      if( !desc )
         return;
      
      id <MTLRenderCommandEncoder> commandEncoder = [metalRenderPass->getCommandBuffer() renderCommandEncoderWithDescriptor:desc];
      commandEncoder.label = @"MyRenderEncoder";
      
      id<MTLRenderPipelineState> renderState = metalRenderState.getPipelineState();
      if( renderState )
         [commandEncoder setRenderPipelineState:renderState];
      
      unsigned int indx {0};
      for( auto& buffer : dataBuffers ) {
         MetalDataBuffer* mtlBuf = dynamic_cast<MetalDataBuffer*>(buffer.get());
         [commandEncoder setVertexBuffer:mtlBuf->getMTLBuffer() offset:0 atIndex:indx++];
      }
      
      // TMP hack for testing
      [commandEncoder drawPrimitives:MTLPrimitiveTypePoint vertexStart:0 vertexCount:1 instanceCount:2000000 baseInstance:0];
      // end hack
      
      [commandEncoder endEncoding];
   }
private:
   
};
std::shared_ptr<IRenderCommand> CreateMetalRenderCommand() {
   return std::make_shared<MetalRenderCommand>();
}



std::unique_ptr<IEventSampler> CreateOSXEventSampler() {
   return std::make_unique<OSXEventSampler>();
}






