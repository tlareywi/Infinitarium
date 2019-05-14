#include "Module.hpp"

#if defined(WIN32)
static const char* libRenderer = "libIERenderer.dll";
static const char* libPlatform = "libIEPlatform.dll";
#else
static const char* libRenderer = "libIERenderer.dylib";
static const char* libPlatform = "libIEPlatform.dylib";
#endif

static void* getSym(void* handle, const char* name) {
#if defined(WIN32)
	return GetProcAddress((HMODULE)handle, name);
#else
	return dlsym(handle, name);
#endif
}

RendererFactory::RendererFactory() : ModuleFactory<RendererFactory>(libRenderer) {
   
   createRenderCommand = (RenderCommandImpl)getSym( handle, "CreateRenderCommand" );
   if( !createRenderCommand ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderCommand!");
   }
   
   createRenderState = (RenderStateImpl)getSym( handle, "CreateRenderState" );
   if( !createRenderState ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderState!");
   }
   
   createRenderPass = (RenderPassImpl)getSym( handle, "CreateRenderPass" );
   if( !createRenderPass ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderPass!");
   }
   
   createRenderPassCopy = (RenderPassImplCopy)getSym( handle, "CreateRenderPassCopy" );
   if( !createRenderPassCopy ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderPassCopy!");
   }
   
   createRenderProgram = (RenderProgramImpl)getSym( handle, "CreateRenderProgram" );
   if( !createRenderProgram ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderProgram!");
   }
   
   createDataBuffer = (DataBufferImpl)getSym( handle, "CreateDataBuffer" );
   if( !createDataBuffer ) {
      throw std::runtime_error("CRITICAL: No implementation for DataBuffer!");
   }
   
   createRenderContext = (RenderContextImpl)getSym( handle, "CreateRenderContext" );
   if( !createRenderContext ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderContext!");
   }
   
   createTexture = (TextureImpl)getSym( handle, "CreateTexture" );
   if( !createTexture ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateTexture!");
   }
   
   cloneTexture = (textureCloneImpl)getSym( handle, "CloneTexture" );
   if( !cloneTexture ) {
      throw std::runtime_error("CRITICAL: No implementation for CloneTexture!");
   }
   
   createRenderTarget = (RenderTargetImpl)getSym( handle, "CreateRenderTarget" );
   if( !createRenderTarget ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateRenderTarget!");
   }
   
   createRenderTargetCopy = (RenderTargetImplCopy)getSym( handle, "CloneRenderTarget" );
   if( !createRenderTargetCopy ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateRenderTargetCopy!");
   }
   
   cloneRenderContext = (RenderContextClone)getSym( handle, "CloneRenderContext" );
   if( !cloneRenderContext ) {
      throw std::runtime_error("CRITICAL: No implementation for CloneRenderContext!");
   }
}

PlatformFactory::PlatformFactory() : ModuleFactory<PlatformFactory>(libPlatform) {
   
   createEventSampler = (EventSamplerImpl)getSym( handle, "CreateEventSampler" );
   if( !createEventSampler ) {
      throw std::runtime_error("CRITICAL: No implementation for EventSampler!");
   }
   
   createApplication = (ApplicationImpl)getSym( handle, "CreateApplication" );
   if( !createApplication ) {
      throw std::runtime_error("CRITICAL: No implementation for Application!");
   }
   
   createApplicationWindow = (ApplicationWindowImpl)getSym( handle, "CreateApplicationWindow" );
   if( !createApplicationWindow ) {
      throw std::runtime_error("CRITICAL: No implementation for ApplicationWindow!");
   }
   
   cloneApplicationWindow = (ApplicationWindowClone)getSym( handle, "CloneApplicationWindow" );
   if( !cloneApplicationWindow ) {
      throw std::runtime_error("CRITICAL: No implementation for CloneApplicationWindow!");
   }
}

