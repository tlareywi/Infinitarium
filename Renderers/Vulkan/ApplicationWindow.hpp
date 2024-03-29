//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "../../Engine/ApplicationWindow.hpp"

#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class WinApplicationWindow : public IApplicationWindow {
public:
	WinApplicationWindow();
	WinApplicationWindow(const IApplicationWindow& obj) : 
		window(window),
		monitor(monitor),
		surface(surface),
		IApplicationWindow(obj) {
	};
	virtual ~WinApplicationWindow();

	void init(IRenderContext&) override;

	void* getPlatformWindow() override {
		return (void*)window;
	}

	void toggleFullScreen(IRenderContext& renderContext) override;

	void* getPlatformSurface() override {
		return (void*)surface;
	}

protected:
	GLFWwindow* window;
	VkSurfaceKHR surface;
	GLFWmonitor* monitor;

	int wndPos[2]{0,0};
	int wndSize[2]{0,0};
};
