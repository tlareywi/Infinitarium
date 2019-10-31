#include "Module.hpp"

#if defined(WIN32)
static const char* libRenderer = "IERenderer.dll";
static const char* libPlatform = "IEPlatform.dll";
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
   
   createRenderCommand = (RenderCommandImpl)getSym( handle, "?CreateRenderCommand@@YA?AV?$shared_ptr@VIRenderCommand@@@std@@XZ" );
   if( !createRenderCommand ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderCommand!");
   }
   
   createRenderState = (RenderStateImpl)getSym( handle, "?CreateRenderState@@YA?AV?$shared_ptr@VIRenderState@@@std@@XZ" );
   if( !createRenderState ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderState!");
   }
   
   createRenderPass = (RenderPassImpl)getSym( handle, "?CreateRenderPass@@YA?AV?$shared_ptr@VIRenderPass@@@std@@XZ" );
   if( !createRenderPass ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderPass!");
   }
   
   createRenderPassCopy = (RenderPassImplCopy)getSym( handle, "?CreateRenderPassCopy@@YA?AV?$shared_ptr@VIRenderPass@@@std@@AEBVIRenderPass@@@Z" );
   if( !createRenderPassCopy ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderPassCopy!");
   }
   
   createRenderProgram = (RenderProgramImpl)getSym( handle, "?CreateRenderProgram@@YA?AV?$shared_ptr@VIRenderProgram@@@std@@XZ" );
   if( !createRenderProgram ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderProgram!");
   }
   
   createDataBuffer = (DataBufferImpl)getSym( handle, "?CreateDataBuffer@@YA?AV?$shared_ptr@VIDataBuffer@@@std@@XZ" );
   if( !createDataBuffer ) {
      throw std::runtime_error("CRITICAL: No implementation for DataBuffer!");
   }
   
   createRenderContext = (RenderContextImpl)getSym( handle, "?CreateRenderContext@@YA?AV?$shared_ptr@VIRenderContext@@@std@@IIII_N@Z" );
   if( !createRenderContext ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderContext!");
   }
   
   createTexture = (TextureImpl)getSym( handle, "?CreateTexture@@YA?AV?$shared_ptr@VITexture@@@std@@AEBU?$vec@$01I$0A@@glm@@W4Format@ITexture@@@Z" );
   if( !createTexture ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateTexture!");
   }
   
   cloneTexture = (textureCloneImpl)getSym( handle, "?CloneTexture@@YA?AV?$shared_ptr@VITexture@@@std@@AEBVITexture@@@Z" );
   if( !cloneTexture ) {
      throw std::runtime_error("CRITICAL: No implementation for CloneTexture!");
   }
   
   createRenderTarget = (RenderTargetImpl)getSym( handle, "?CreateRenderTarget@@YA?AV?$shared_ptr@VIRenderTarget@@@std@@AEBU?$vec@$01I$0A@@glm@@W4Format@ITexture@@W4Type@IRenderTarget@@W4Resource@8@@Z" );
   if( !createRenderTarget ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateRenderTarget!");
   }
   
   createRenderTargetCopy = (RenderTargetImplCopy)getSym( handle, "?CloneRenderTarget@@YA?AV?$shared_ptr@VIRenderTarget@@@std@@AEBVIRenderTarget@@@Z" );
   if( !createRenderTargetCopy ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateRenderTargetCopy!");
   }
   
   cloneRenderContext = (RenderContextClone)getSym( handle, "?CloneRenderContext@@YA?AV?$shared_ptr@VIRenderContext@@@std@@AEBVIRenderContext@@@Z" );
   if( !cloneRenderContext ) {
      throw std::runtime_error("CRITICAL: No implementation for CloneRenderContext!");
   }
}

PlatformFactory::PlatformFactory() : ModuleFactory<PlatformFactory>(libPlatform) {
   
   createEventSampler = (EventSamplerImpl)getSym( handle, "?CreateEventSampler@@YA?AV?$shared_ptr@VIEventSampler@@@std@@XZ" );
   if( !createEventSampler ) {
      throw std::runtime_error("CRITICAL: No implementation for EventSampler!");
   }
   
   createApplication = (ApplicationImpl)getSym( handle, "?CreateApplication@@YA?AV?$shared_ptr@VIApplication@@@std@@XZ" );
   if( !createApplication ) {
      throw std::runtime_error("CRITICAL: No implementation for Application!");
   }
   
   createApplicationWindow = (ApplicationWindowImpl)getSym( handle, "?CreateApplicationWindow@@YA?AV?$shared_ptr@VIApplicationWindow@@@std@@XZ" );
   if( !createApplicationWindow ) {
      throw std::runtime_error("CRITICAL: No implementation for ApplicationWindow!");
   }
   
   cloneApplicationWindow = (ApplicationWindowClone)getSym( handle, "?CloneApplicationWindow@@YA?AV?$shared_ptr@VIApplicationWindow@@@std@@AEBVIApplicationWindow@@@Z" );
   if( !cloneApplicationWindow ) {
      throw std::runtime_error("CRITICAL: No implementation for CloneApplicationWindow!");
   }
}

