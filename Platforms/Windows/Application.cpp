#include "Application.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

std::shared_ptr<IApplication> WinApplication::instance{ nullptr };

std::shared_ptr<IApplication> WinApplication::Instance() {
	if (!instance)
		instance = std::make_shared<WinApplication>();

	return instance;
}

WinApplication::WinApplication() {
	glfwInit();
}

void WinApplication::run() {
	while( 1 ) {
		glfwPollEvents();
	}
}

void WinApplication::stop() {
	glfwTerminate();
}

void WinApplication::addManipulator(const std::string& id, float, float, float) {

}

__declspec(dllexport) std::shared_ptr<IApplication> CreateApplication() {
	return WinApplication::Instance();
}
