#include "ApplicationWindow.hpp"
#include "../../Engine/EventSampler.hpp"

WinApplicationWindow::WinApplicationWindow() : window(nullptr) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: make resizeable for desktop mode
}

void WinApplicationWindow::init( IRenderContext& renderContext ) {
	window = glfwCreateWindow(renderContext.width(), renderContext.height(), "Vulkan", nullptr, nullptr);
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