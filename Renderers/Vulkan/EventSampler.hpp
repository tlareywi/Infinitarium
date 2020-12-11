#pragma once

#include "../../Engine/EventSampler.hpp"
#include "../../Engine/RenderContext.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <chrono>

///
/// brief Windows implementation of IEventSampler
///
class WindowsEventSampler : public IEventSampler {
public:
	WindowsEventSampler() : leftButtonDown(false), rightButtonDown(false) {};

	void setTargetWindow(GLFWwindow*, IRenderContext&);

private:
	void onMouseButton(GLFWwindow* window, int button, int action, int mods);
	void onMouseMove(GLFWwindow* window, double xpos, double ypos);
	void onFramebufferSize(GLFWwindow* window, int w, int h);

	std::chrono::high_resolution_clock::time_point mouseLeftTimer;
	std::chrono::high_resolution_clock::time_point mouseRightTimer;
	std::chrono::high_resolution_clock::time_point mouseMiddleTimer;

	bool leftButtonDown;
	bool rightButtonDown;
	double lastX;
	double lastY;
	
	// TODO: Janky, fix.
	IRenderContext* context;
};

