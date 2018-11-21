//
//  DataBuffer.cpp
//  Infinitarium
//
//  Created by Trystan Larey-Williams on 10/13/18.
//

#include "EngineImpl.hpp"
#include "ShaderTypes.h"

#import <Metal/Metal.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

#include "../../Engine/RenderCommand.hpp"
#include "../../Engine/RenderProgram.hpp"
#include "../../Engine/RenderContext.hpp"

#include <sstream>
#include <fstream>

#include "../../config.h"

///
///
///
class MetalRenderContext : public IRenderContext {
public:
   MetalRenderContext( unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs ) : IRenderContext(x, y, w, h, fs) {
      device = MTLCreateSystemDefaultDevice();
      surface = [[CAMetalLayer new] init];
      CGRect newFrame = CGRectMake( x, y, w, h );
      
      surface.pixelFormat = MTLPixelFormatBGRA8Unorm;
      surface.device = device;
      surface.framebufferOnly = YES;
      surface.frame = newFrame;
      surface.displaySyncEnabled = NO;
      surface.drawableSize = newFrame.size;
      
      commandQ = [device newCommandQueue];
   }
   
   id<MTLDevice> getMTLDevice() {
      return device;
   }
   
   id<MTLCommandQueue> getMTLCommandQ() {
      return commandQ;
   }
   
   CAMetalLayer* getMTLLayer() {
      return surface;
   }
   
   CGRect getCGRect() {
      return rect;
   }
   
   void* getSurface() override {
      return (void*)surface;
   }
   
private:
   id<MTLDevice> device;
   id<MTLCommandQueue> commandQ;
   CAMetalLayer* surface;
   CGRect rect;
};

extern "C" {
   std::shared_ptr<IRenderContext> CreateRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs) {
      std::shared_ptr<IRenderContext> context = std::make_shared<MetalRenderContext>(x, y, w, h, fs);
      return context;
   }
}


///
/// brief Metal implementation of DataBuffer
///
class MetalDataBuffer : public IDataBuffer {
public:
   MetalDataBuffer( IRenderContext& context ) {
      MetalRenderContext* c = dynamic_cast<MetalRenderContext*>( &context );
      device = c->getMTLDevice();
      commandQ = c->getMTLCommandQ();
   }
   
   void set( DataPackContainer& container ) override {
      std::visit( [this](auto& e) { // TODO: move visit to caller level and pass DataPack instead?
         @autoreleasepool {
            id<MTLBuffer> tmpBuffer = [device newBufferWithBytes:e.get() length:e.sizeBytes() options:MTLResourceStorageModeManaged];
            
            GPU = [device newBufferWithLength:e.sizeBytes() options:MTLResourceStorageModePrivate];
            
            id<MTLCommandBuffer> cmdBuf = [commandQ commandBuffer];
            id<MTLBlitCommandEncoder> bltEncoder = [cmdBuf blitCommandEncoder];
            [bltEncoder copyFromBuffer:tmpBuffer sourceOffset:0 toBuffer:GPU destinationOffset:0 size:e.sizeBytes()];
            [bltEncoder endEncoding];
            [cmdBuf commit];
         }
      }, container );
   };
   
   void reserve( unsigned int sizeBytes ) override {
      GPU = [device newBufferWithLength:sizeBytes options:MTLResourceStorageModePrivate];
   }
   
   void set( const void* const data, unsigned int sizeBytes ) override {
      @autoreleasepool {
         if( !GPU ) {
            GPU = [device newBufferWithLength:sizeBytes options:MTLResourceStorageModePrivate];
         }
         
         id<MTLBuffer> tmpBuffer = [device newBufferWithBytes:data length:sizeBytes options:MTLResourceStorageModeManaged];
         
         id<MTLCommandBuffer> cmdBuf = [commandQ commandBuffer];
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
   id<MTLDevice> device;
   id<MTLCommandQueue> commandQ;
};

extern "C" {
   std::shared_ptr<IDataBuffer> CreateDataBuffer( IRenderContext& context ) {
      std::shared_ptr<MetalDataBuffer> databuf = std::make_shared<MetalDataBuffer>(context);
      return databuf;
   }
}


///
/// brief Metal implementation of RendePass
///
class MetalRenderPass : public IRenderPass {
public:
   void begin() override {
      MetalRenderContext* context = dynamic_cast<MetalRenderContext*>( renderContext.get() );
      
      commandBuffer = [context->getMTLCommandQ() commandBuffer];
      commandBuffer.label = @"IRenderPass";
      
      CAMetalLayer* layer = context->getMTLLayer();
      
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

extern "C" {
   std::shared_ptr<IRenderPass> CreateRenderPass() {
      return std::make_shared<MetalRenderPass>();
   }
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
   
   void commit( IRenderContext& context ) override {
      MetalRenderContext* c = dynamic_cast<MetalRenderContext*>( &context );
      id<MTLDevice> device = c->getMTLDevice();
      
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

extern "C" {
   std::shared_ptr<IRenderState> CreateRenderState() {
      return std::make_shared<MetalRenderState>();
   }
}


///
/// brief Metal implementation of RenderProgram
///
class MetalRenderProgram : public IRenderProgram {
   void prepare( IRenderState& state ) override {
      // Separating this out from compile allows function switching within a library
      // without recompiling the shader source.
      vertex = [program newFunctionWithName:@"staticInstancedStarsVert"];
      fragment = [program newFunctionWithName:@"staticInstancedStarsFrag"];
      
      MetalRenderState* metalState = dynamic_cast<MetalRenderState*>( &state );
      metalState->getPipelineDescriptor().vertexFunction = vertex;
      metalState->getPipelineDescriptor().fragmentFunction = fragment;
   }
   
   void apply( IRenderState& state ) override {
   }
   
   void compile( const std::string& name, IRenderContext& context ) override {
      MetalRenderContext* c = dynamic_cast<MetalRenderContext*>( &context );
      id<MTLDevice> device = c->getMTLDevice();
      
      std::string path {std::string(INSTALL_ROOT) + std::string("/share/Infinitarium/shaders/metal/") + name + ".metal"};
      
      std::ifstream infile(path);
      std::stringstream buffer;
      buffer << infile.rdbuf();
      
      NSError* err {nullptr};
      program = [device newLibraryWithSource:[NSString stringWithUTF8String:buffer.str().c_str()] options:nullptr error:&err];
      
      infile.close();
      
      if( err ) {
         std::cout<<"Error compiling "<<path<<" "<<[err.localizedDescription UTF8String]<<std::endl;
      }
   }
   
private:
   id<MTLFunction> vertex;
   id<MTLFunction> fragment;
   id<MTLLibrary> program;
};

extern "C" {
   std::shared_ptr<IRenderProgram> CreateRenderProgram() {
      return std::make_shared<MetalRenderProgram>();
   }
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
      [commandEncoder drawPrimitives:MTLPrimitiveTypePoint vertexStart:0 vertexCount:1 instanceCount:2539802 baseInstance:0];
      // end hack
      
      [commandEncoder endEncoding];
   }
private:
   
};

extern "C" {
   std::shared_ptr<IRenderCommand> CreateRenderCommand() {
      return std::make_shared<MetalRenderCommand>();
   }
}








