#include "Module.hpp"

RendererFactory::RendererFactory() : ModuleFactory<RendererFactory>("libIERenderer.dylib") {
   
   createRenderCommand = (RenderCommandImpl)dlsym( handle, "CreateRenderCommand" );
   if( !createRenderCommand ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderCommand!");
   }
   
   createRenderState = (RenderStateImpl)dlsym( handle, "CreateRenderState" );
   if( !createRenderState ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderState!");
   }
   
   createRenderPass = (RenderPassImpl)dlsym( handle, "CreateRenderPass" );
   if( !createRenderPass ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderPass!");
   }
   
   createRenderPassCopy = (RenderPassImplCopy)dlsym( handle, "CreateRenderPassCopy" );
   if( !createRenderPassCopy ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderPassCopy!");
   }
   
   createRenderProgram = (RenderProgramImpl)dlsym( handle, "CreateRenderProgram" );
   if( !createRenderProgram ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderProgram!");
   }
   
   createDataBuffer = (DataBufferImpl)dlsym( handle, "CreateDataBuffer" );
   if( !createDataBuffer ) {
      throw std::runtime_error("CRITICAL: No implementation for DataBuffer!");
   }
   
   createRenderContext = (RenderContextImpl)dlsym( handle, "CreateRenderContext" );
   if( !createRenderContext ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderContext!");
   }
   
   createTexture = (TextureImpl)dlsym( handle, "CreateTexture" );
   if( !createTexture ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateTexture!");
   }
   
   createRenderTarget = (RenderTargetImpl)dlsym( handle, "CreateRenderTarget" );
   if( !createRenderTarget ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateRenderTarget!");
   }
   
   createRenderTargetCopy = (RenderTargetImplCopy)dlsym( handle, "CreateRenderTargetCopy" );
   if( !createRenderTargetCopy ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateRenderTargetCopy!");
   }
   
   cloneRenderContext = (RenderContextClone)dlsym( handle, "CloneRenderContext" );
   if( !cloneRenderContext ) {
      throw std::runtime_error("CRITICAL: No implementation for CloneRenderContext!");
   }
}

PlatformFactory::PlatformFactory() : ModuleFactory<PlatformFactory>("libIEPlatform.dylib") {
   
   createEventSampler = (EventSamplerImpl)dlsym( handle, "CreateEventSampler" );
   if( !createEventSampler ) {
      throw std::runtime_error("CRITICAL: No implementation for EventSampler!");
   }
   
   createApplication = (ApplicationImpl)dlsym( handle, "CreateApplication" );
   if( !createApplication ) {
      throw std::runtime_error("CRITICAL: No implementation for Application!");
   }
   
   createApplicationWindow = (ApplicationWindowImpl)dlsym( handle, "CreateApplicationWindow" );
   if( !createApplicationWindow ) {
      throw std::runtime_error("CRITICAL: No implementation for ApplicationWindow!");
   }
   
   cloneApplicationWindow = (ApplicationWindowClone)dlsym( handle, "CloneApplicationWindow" );
   if( !cloneApplicationWindow ) {
      throw std::runtime_error("CRITICAL: No implementation for CloneApplicationWindow!");
   }
}

