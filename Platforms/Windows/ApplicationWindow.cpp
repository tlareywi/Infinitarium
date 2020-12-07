#define VK_USE_PLATFORM_WIN32_KHR 

#include "Application.hpp"
#include "ApplicationWindow.hpp"
#include "EventSampler.hpp"
#include "../../Renderers/Vulkan/OpenXRContext.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

std::shared_ptr<IEventSampler> CreateEventSampler();

///////////////////////////////////////////////////////////////////////////////
// WinApplicationWindow
///////////////////////////////////////////////////////////////////////////////

WinApplicationWindow::WinApplicationWindow() : window(nullptr), surface(VK_NULL_HANDLE) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

WinApplicationWindow::~WinApplicationWindow() {
	WinApplication& instance{ dynamic_cast<WinApplication&>(*WinApplication::Instance()) };
	vkDestroySurfaceKHR(instance.getVkInstance(), surface, nullptr);
	glfwDestroyWindow(window);
}

void WinApplicationWindow::init( IRenderContext& renderContext ) {
	bool headset = dynamic_cast<OpenXRContext*>(&renderContext) ? true : false;
	WinApplication& instance = dynamic_cast<WinApplication&>(*WinApplication::Instance());

	OpenXRContext::ContextParams params = { nullptr, nullptr, nullptr, 0 };
	// CLUDGE
	if (headset) {
		params.xrInstance = instance.getXrInstance();
		params.xrSystemId = instance.getXrSystemId();
	}
	else {
		if (renderContext.fullScreen()) {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			window = glfwCreateWindow(mode->width, mode->height, "Infinitarium", monitor, nullptr);

			renderContext.setContextExtent(mode->width, mode->height);
		}
		else
			window = glfwCreateWindow(renderContext.width(), renderContext.height(), "Infinitarium", nullptr, nullptr);

		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = glfwGetWin32Window(window);
		createInfo.hinstance = GetModuleHandle(nullptr);

		if (vkCreateWin32SurfaceKHR(instance.getVkInstance(), &createInfo, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan window surface!");
		}

		params.surface = surface;
		std::shared_ptr<IEventSampler> eventSampler{ CreateEventSampler() };
		WindowsEventSampler& winEvents{ dynamic_cast<WindowsEventSampler&>(*eventSampler) };
		winEvents.setTargetWindow(window, renderContext);
	}

	params.vkInstance = instance.getVkInstance();

	renderContext.setSurface(&params);
}

__declspec(dllexport) std::shared_ptr<IApplicationWindow> CreateApplicationWindow() {
	std::shared_ptr<IApplicationWindow> window = std::make_shared<WinApplicationWindow>();
	return window;
}

__declspec(dllexport) std::shared_ptr<IApplicationWindow> CloneApplicationWindow(const IApplicationWindow& obj) {
	std::shared_ptr<IApplicationWindow> window = std::make_shared<WinApplicationWindow>(obj);
	return window;
}

