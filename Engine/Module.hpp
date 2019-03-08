#pragma once

#include <memory>
#include <string>

#include <dlfcn.h>

#include "RenderCommand.hpp"
#include "RenderState.hpp"
#include "RenderPass.hpp"
#include "RenderProgram.hpp"
#include "DataBuffer.hpp"
#include "MotionControllerOrbit.hpp"
#include "Application.hpp"
#include "ApplicationWindow.hpp"
#include "Texture.hpp"

///
/// \brief Encapsulate some boiler-plate aspects of runtime module loading. Enforces
/// one copy of module referenced (singleton).
///
template<typename T> class ModuleFactory {
public:
   virtual ~ModuleFactory() {
      if( handle ) {
         dlclose(handle);
         handle = nullptr;
      }
   }
   
   static std::shared_ptr<T> Instance() {
      if( instance == nullptr ) {
         instance = std::make_shared<T>();
      }
      
      return instance;
   }
   
protected:
   ModuleFactory( const std::string& module ) {
      handle = dlopen( module.c_str(), RTLD_LOCAL|RTLD_LAZY );
      if( !handle ) {
         throw std::runtime_error("CRITICAL: Unable to load module!");
      }
   }
   
   void* handle;
   
private:
   ModuleFactory();
   
   static std::shared_ptr<T> instance;
};

template<typename T> std::shared_ptr<T> ModuleFactory<T>::instance = nullptr;

///
/// \brief Provides objects that implement specific rendering backends (Metal vs. Vulkan).
///
class RendererFactory : public ModuleFactory<RendererFactory> {
public:
   RendererFactory();
   
   typedef std::shared_ptr<IRenderCommand> (*RenderCommandImpl)();
   typedef std::shared_ptr<IRenderState> (*RenderStateImpl)();
   typedef std::shared_ptr<IRenderPass> (*RenderPassImpl)();
   typedef std::shared_ptr<IRenderPass> (*RenderPassImplCopy)( const IRenderPass& );
   typedef std::shared_ptr<IRenderProgram> (*RenderProgramImpl)();
   typedef std::shared_ptr<IDataBuffer> (*DataBufferImpl)( IRenderContext& );
   typedef std::shared_ptr<IRenderContext> (*RenderContextImpl)( unsigned int, unsigned int, unsigned int, unsigned int, bool );
   typedef std::shared_ptr<ITexture> (*TextureImpl)( const glm::uvec2&, ITexture::Format );
   typedef std::shared_ptr<IRenderTarget> (*RenderTargetImpl)( const glm::uvec2&, ITexture::Format, IRenderTarget::Type, IRenderTarget::Resource );
   typedef std::shared_ptr<IRenderTarget> (*RenderTargetImplCopy)( const IRenderTarget& );
      
   virtual ~RendererFactory() {
      createRenderCommand = nullptr;
      createRenderState = nullptr;
      createRenderPass = nullptr;
      createRenderProgram = nullptr;
      createDataBuffer = nullptr;
      createRenderContext = nullptr;
      createTexture = nullptr;
      createRenderTarget = nullptr;
      createRenderPassCopy = nullptr;
      createRenderTargetCopy = nullptr;
   }
   
   RenderCommandImpl createRenderCommand;
   RenderStateImpl createRenderState;
   RenderPassImpl createRenderPass;
   RenderProgramImpl createRenderProgram;
   DataBufferImpl createDataBuffer;
   RenderContextImpl createRenderContext;
   TextureImpl createTexture;
   RenderTargetImpl createRenderTarget;
   RenderTargetImplCopy createRenderTargetCopy;
   RenderPassImplCopy createRenderPassCopy;
};

///
/// \brief Provides objects that implement platform specific UI/Event/Windowing.
///
class PlatformFactory : public ModuleFactory<PlatformFactory> {
public:
   PlatformFactory();
   
   typedef std::shared_ptr<IEventSampler> (*EventSamplerImpl)();
   typedef std::shared_ptr<IApplicationWindow> (*ApplicationWindowImpl)( IRenderContext& );
   typedef std::shared_ptr<IApplication> (*ApplicationImpl)();
   
   virtual ~PlatformFactory() {
      createEventSampler = nullptr;
      createApplication = nullptr;
      createApplicationWindow = nullptr;
   }
   
   EventSamplerImpl createEventSampler;
   ApplicationImpl createApplication;
   ApplicationWindowImpl createApplicationWindow;
};

