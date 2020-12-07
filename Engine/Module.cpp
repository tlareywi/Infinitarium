#include "Module.hpp"

#include <map>

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

#if defined(WIN32)
static std::map<std::string, std::string> symbolMap {
   {"CreateEventSampler", "?CreateEventSampler@@YA?AV?$shared_ptr@VIEventSampler@@@std@@XZ"},
   {"CreateApplicationWindow", "?CreateApplicationWindow@@YA?AV?$shared_ptr@VIApplicationWindow@@@std@@XZ"},
   {"CloneApplicationWindow", "?CloneApplicationWindow@@YA?AV?$shared_ptr@VIApplicationWindow@@@std@@AEBVIApplicationWindow@@@Z"},
   {"CreateApplication", "?CreateApplication@@YA?AV?$shared_ptr@VIApplication@@@std@@XZ"},
   {"CreateRenderCommand", "?CreateRenderCommand@@YA?AV?$shared_ptr@VIRenderCommand@@@std@@XZ"},
   {"CreateRenderState", "?CreateRenderState@@YA?AV?$shared_ptr@VIRenderState@@@std@@XZ"},
   {"CreateRenderPass", "?CreateRenderPass@@YA?AV?$shared_ptr@VIRenderPass@@@std@@XZ"},
   {"CreateRenderPassCopy", "?CreateRenderPassCopy@@YA?AV?$shared_ptr@VIRenderPass@@@std@@AEBVIRenderPass@@@Z"},
   {"CreateRenderProgram", "?CreateRenderProgram@@YA?AV?$shared_ptr@VIRenderProgram@@@std@@XZ"},
   {"CreateDataBuffer", "?CreateDataBuffer@@YA?AV?$shared_ptr@VIDataBuffer@@@std@@AEAVIRenderContext@@@Z"},
   {"CreateRenderContext", "?CreateRenderContext@@YA?AV?$shared_ptr@VIRenderContext@@@std@@IIII_N0@Z"},
   {"CloneRenderContext", "?CloneRenderContext@@YA?AV?$shared_ptr@VIRenderContext@@@std@@AEBVIRenderContext@@@Z"},
   {"CreateTexture", "?CreateTexture@@YA?AV?$shared_ptr@VITexture@@@std@@AEBU?$vec@$01I$0A@@glm@@W4Format@ITexture@@@Z"},
   {"CloneTexture", "?CloneTexture@@YA?AV?$shared_ptr@VITexture@@@std@@AEBVITexture@@@Z"},
   {"CreateRenderTarget", "?CreateRenderTarget@@YA?AV?$shared_ptr@VIRenderTarget@@@std@@AEBU?$vec@$01I$0A@@glm@@W4Format@ITexture@@W4Type@IRenderTarget@@W4Resource@8@@Z"},
   {"CloneRenderTarget", "?CloneRenderTarget@@YA?AV?$shared_ptr@VIRenderTarget@@@std@@AEBVIRenderTarget@@@Z"},
   {"CreateImGUI", "?CreateImGUI@@YA?AV?$shared_ptr@VIImGUI@@@std@@XZ"}
};
#else
static std::map<std::string, std::string> symbolMap {
   {"CreateEventSampler", "_Z18CreateEventSamplerv"},
   {"CreateApplicationWindow", "_Z23CreateApplicationWindowv"},
   {"CloneApplicationWindow", "_Z22CloneApplicationWindowRK18IApplicationWindow"},
   {"CreateApplication", "_Z17CreateApplicationv"},
   {"CreateRenderCommand", "_Z19CreateRenderCommandv"},
   {"CreateRenderState", "_Z17CreateRenderStatev"},
   {"CreateRenderPass", "_Z16CreateRenderPassv"},
   {"CreateRenderPassCopy", "_Z20CreateRenderPassCopyRK11IRenderPass"},
   {"CreateRenderProgram", "_Z19CreateRenderProgramv"},
   {"CreateDataBuffer", "_Z16CreateDataBufferR14IRenderContext"},
   {"CreateRenderContext", "_Z19CreateRenderContextjjjjb"},
   {"CloneRenderContext", "_Z18CloneRenderContextRK14IRenderContext"},
   {"CreateTexture", "_Z13CreateTextureRKN3glm3vecILi2EjLNS_9qualifierE0EEEN8ITexture6FormatE"},
   {"CloneTexture", "_Z12CloneTextureRK8ITexture"},
   {"CreateRenderTarget", "_Z18CreateRenderTargetRKN3glm3vecILi2EjLNS_9qualifierE0EEEN8ITexture6FormatEN13IRenderTarget4TypeENS7_8ResourceE"},
   {"CloneRenderTarget", "_Z17CloneRenderTargetRK13IRenderTarget"}
};
#endif

RendererFactory::RendererFactory() : ModuleFactory<RendererFactory>(libRenderer) {
   
   createRenderCommand = (RenderCommandImpl)getSym( handle, symbolMap["CreateRenderCommand"].c_str() );
   if( !createRenderCommand ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderCommand!");
   }
   
   createRenderState = (RenderStateImpl)getSym( handle, symbolMap["CreateRenderState"].c_str() );
   if( !createRenderState ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderState!");
   }
   
   createRenderPass = (RenderPassImpl)getSym( handle, symbolMap["CreateRenderPass"].c_str() );
   if( !createRenderPass ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderPass!");
   }
   
   createRenderPassCopy = (RenderPassImplCopy)getSym( handle, symbolMap["CreateRenderPassCopy"].c_str() );
   if( !createRenderPassCopy ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderPassCopy!");
   }
   
   createRenderProgram = (RenderProgramImpl)getSym( handle, symbolMap["CreateRenderProgram"].c_str() );
   if( !createRenderProgram ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderProgram!");
   }
   
   createDataBuffer = (DataBufferImpl)getSym( handle, symbolMap["CreateDataBuffer"].c_str() );
   if( !createDataBuffer ) {
      throw std::runtime_error("CRITICAL: No implementation for DataBuffer!");
   }
   
   createRenderContext = (RenderContextImpl)getSym( handle, symbolMap["CreateRenderContext"].c_str() );
   if( !createRenderContext ) {
      throw std::runtime_error("CRITICAL: No implementation for RenderContext!");
   }
   
   cloneRenderContext = (RenderContextClone)getSym( handle, symbolMap["CloneRenderContext"].c_str() );
   if( !cloneRenderContext ) {
      throw std::runtime_error("CRITICAL: No implementation for CloneRenderContext!");
   }
   
   createTexture = (TextureImpl)getSym( handle, symbolMap["CreateTexture"].c_str() );
   if( !createTexture ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateTexture!");
   }
   
   cloneTexture = (textureCloneImpl)getSym( handle, symbolMap["CloneTexture"].c_str() );
   if( !cloneTexture ) {
      throw std::runtime_error("CRITICAL: No implementation for CloneTexture!");
   }
   
   createRenderTarget = (RenderTargetImpl)getSym( handle, symbolMap["CreateRenderTarget"].c_str() );
   if( !createRenderTarget ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateRenderTarget!");
   }
   
   createRenderTargetCopy = (RenderTargetImplCopy)getSym( handle, symbolMap["CloneRenderTarget"].c_str() );
   if( !createRenderTargetCopy ) {
      throw std::runtime_error("CRITICAL: No implementation for CreateRenderTargetCopy!");
   }

   createImGUI = (ImGUIImpl)getSym(handle, symbolMap["CreateImGUI"].c_str());
   if (!createImGUI) {
       throw std::runtime_error("CRITICAL: No implementation for CreateImGUI!");
   }
}

PlatformFactory::PlatformFactory() : ModuleFactory<PlatformFactory>(libPlatform) {
   
   createEventSampler = (EventSamplerImpl)getSym( handle, symbolMap["CreateEventSampler"].c_str() );
   if( !createEventSampler ) {
      throw std::runtime_error("CRITICAL: No implementation for EventSampler!");
   }
   
   createApplication = (ApplicationImpl)getSym( handle, symbolMap["CreateApplication"].c_str() );
   if( !createApplication ) {
      throw std::runtime_error("CRITICAL: No implementation for Application!");
   }
   
   createApplicationWindow = (ApplicationWindowImpl)getSym( handle, symbolMap["CreateApplicationWindow"].c_str() );
   if( !createApplicationWindow ) {
      throw std::runtime_error("CRITICAL: No implementation for ApplicationWindow!");
   }
   
   cloneApplicationWindow = (ApplicationWindowClone)getSym( handle, symbolMap["CloneApplicationWindow"].c_str() );
   if( !cloneApplicationWindow ) {
      throw std::runtime_error("CRITICAL: No implementation for CloneApplicationWindow!");
   }
}

