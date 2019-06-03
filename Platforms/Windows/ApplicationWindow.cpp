#define VK_USE_PLATFORM_WIN32_KHR 

#include "Application.hpp"
#include "ApplicationWindow.hpp"
#include "../../Engine/EventSampler.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

WinApplicationWindow::WinApplicationWindow() : window(nullptr), surface(VK_NULL_HANDLE) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: make resizeable for desktop mode
}

WinApplicationWindow::~WinApplicationWindow() {
	WinApplication* instance = dynamic_cast<WinApplication*>(WinApplication::Instance().get());
	vkDestroySurfaceKHR(instance->getVkInstance(), surface, nullptr);
	glfwDestroyWindow(window);
}

void WinApplicationWindow::init( IRenderContext& renderContext ) {
	window = glfwCreateWindow(renderContext.width(), renderContext.height(), "Vulkan", nullptr, nullptr);

	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = glfwGetWin32Window(window);
	createInfo.hinstance = GetModuleHandle(nullptr);

	WinApplication* instance = dynamic_cast<WinApplication*>(WinApplication::Instance().get());

	if (vkCreateWin32SurfaceKHR(instance->getVkInstance(), &createInfo, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan window surface!");
	}

	renderContext.init(*this);
}

__declspec(dllexport) std::shared_ptr<IApplicationWindow> CreateApplicationWindow() {
	std::shared_ptr<IApplicationWindow> window = std::make_shared<WinApplicationWindow>();
	return window;
}

__declspec(dllexport) std::shared_ptr<IApplicationWindow> CloneApplicationWindow(const IApplicationWindow& obj) {
	std::shared_ptr<IApplicationWindow> window = std::make_shared<WinApplicationWindow>(obj);
	return window;
}

class WinEventSampler : public IEventSampler {
public:

private:

};

static std::shared_ptr<IEventSampler> eventSampler = std::make_shared<WinEventSampler>();

__declspec(dllexport) std::shared_ptr<IEventSampler> CreateEventSampler() {
	return eventSampler;
}