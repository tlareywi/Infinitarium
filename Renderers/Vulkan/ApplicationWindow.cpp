//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Application.hpp"
#include "ApplicationWindow.hpp"
#include "EventSampler.hpp"
#include "RenderContext.hpp"

#include <thread>

#if (USING_OPENXR)
	#include "../../Renderers/Vulkan/OpenXRContext.hpp"
#endif

#if defined(WIN32)
	#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
    #define GLFW_EXPOSE_NATIVE_COCOA
#else
	#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h> // TODO: really need native level here? Try using non-native calls on both plats and see if we can get rid of this.

std::shared_ptr<IEventSampler> CreateEventSampler();

///////////////////////////////////////////////////////////////////////////////
// WinApplicationWindow
///////////////////////////////////////////////////////////////////////////////

WinApplicationWindow::WinApplicationWindow() : window(nullptr), surface(VK_NULL_HANDLE), monitor(nullptr) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

WinApplicationWindow::~WinApplicationWindow() {
	WinApplication& instance{ dynamic_cast<WinApplication&>(*WinApplication::Instance()) };
	vkDestroySurfaceKHR(instance.getVkInstance(), surface, nullptr);
	glfwDestroyWindow(window);
}

// TODO We can drop reference to renderContext by passing necessary values. ApplicationWindow doesn't need to know about context. 
// Don't have a problem with circular references yet but I see one on the horizon.
void WinApplicationWindow::init( IRenderContext& renderContext ) {
	WinApplication& instance = dynamic_cast<WinApplication&>(*WinApplication::Instance());

	VulkanRenderContext::ContextParams params{};

#if (USING_OPENXR)
	bool headset = dynamic_cast<OpenXRContext*>(&renderContext) ? true : false;
	// CLUDGE
	if (headset) {
		params.xrInstance = instance.getXrInstance();
		params.xrSystemId = instance.getXrSystemId();
	}
	else 
#endif 
	{
		if (renderContext.fullScreen()) {
			wndPos[0] = renderContext.x();
			wndPos[1] = renderContext.y();
			wndSize[0] = renderContext.width();
			wndSize[1] = renderContext.height();

            monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			glfwWindowHint(GLFW_FOCUSED, 1);
            
			window = glfwCreateWindow(mode->width, mode->height, "Infinitarium", monitor, nullptr);
		}
		else
			window = glfwCreateWindow(renderContext.width(), renderContext.height(), "Infinitarium", nullptr, nullptr);
        
        // hiDPI; GLFW handles this internally, so we need to ask it what the logical size is for our swapchain.
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        renderContext.setContextExtent(width, height);

#if (WIN32)
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = glfwGetWin32Window(window);
		createInfo.hinstance = GetModuleHandle(nullptr);

		if (vkCreateWin32SurfaceKHR(instance.getVkInstance(), &createInfo, nullptr, &surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create Vulkan window surface!");
#else
		if( glfwCreateWindowSurface( instance.getVkInstance(), window, nullptr, &surface ) != VK_SUCCESS )
			throw std::runtime_error("Failed to create Vulkan window surface!");
		
		sleep(1);
#endif

		params.surface = surface;
		std::shared_ptr<IEventSampler> eventSampler{ CreateEventSampler() };
		WindowsEventSampler& winEvents{ dynamic_cast<WindowsEventSampler&>(*eventSampler) };
		winEvents.setTargetWindow(window, renderContext);
	}

	params.vkInstance = instance.getVkInstance();

	renderContext.setSurface(&params);
}

void WinApplicationWindow::toggleFullScreen(IRenderContext& renderContext) {
	if (!monitor) {
        std::atomic<bool> done{false};
        std::function<void()> f = [this, &done, &renderContext](){
            monitor = glfwGetPrimaryMonitor();

            // backup window position and size
            glfwGetWindowPos(window, &wndPos[0], &wndPos[1]);
            glfwGetWindowSize(window, &wndSize[0], &wndSize[1]);

            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            
            // hiDPI; GLFW handles this internally, so we need to ask it what the logical size is for our swapchain.
            int width = 0, height = 0;
            glfwGetFramebufferSize(window, &width, &height);
            renderContext.setContextExtent(width, height);
            done = true;
        };
        CreateApplication()->runOnUIThread( f );
        while( !done )
            std::this_thread::yield();
	}
	else {
		// restore
        std::atomic<bool> done{false};
        std::function<void()> f = [this, &done, &renderContext](){
            monitor = nullptr;
            glfwSetWindowMonitor(window, monitor, wndPos[0], wndPos[1], wndSize[0], wndSize[1], 0);
            int w{0}, h{0};
            glfwGetFramebufferSize(window, &w, &h);
            renderContext.setContextExtent(w, h);
            done = true;
        };
        CreateApplication()->runOnUIThread( f );
        while( !done )
            std::this_thread::yield();
	}
}

RENDERER_EXPORT std::shared_ptr<IApplicationWindow> CreateApplicationWindow() {
	std::shared_ptr<IApplicationWindow> window = std::make_shared<WinApplicationWindow>();
	return window;
}



