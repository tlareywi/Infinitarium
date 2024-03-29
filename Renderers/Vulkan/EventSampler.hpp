//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

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
	WindowsEventSampler() : leftButtonDown(false), rightButtonDown(false), middleButtonDown(false), lastX(0), lastY(0) {};
	virtual ~WindowsEventSampler() {}

	void setTargetWindow(GLFWwindow*, IRenderContext&);

private:
	void onMouseButton(GLFWwindow* window, int button, int action, int mods);
	void onMouseMove(GLFWwindow* window, double xpos, double ypos);
	void onFramebufferSize(GLFWwindow* window, int w, int h);
	void onKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods);
	void onScroll(GLFWwindow* window, double xoffset, double yoffset);

	std::chrono::high_resolution_clock::time_point mouseLeftTimer;
	std::chrono::high_resolution_clock::time_point mouseRightTimer;
	std::chrono::high_resolution_clock::time_point mouseDoubleTimer;

	bool leftButtonDown;
	bool rightButtonDown;
	bool middleButtonDown;
	double lastX;
	double lastY;
	
	// TODO: Janky, fix.
	IRenderContext* context{nullptr};
};


