#pragma once

#include "../../Engine/Application.hpp"

#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class WinApplicationWindow : public IApplicationWindow {
public:
	WinApplicationWindow();
	WinApplicationWindow(const IApplicationWindow& obj) : IApplicationWindow(obj) {};
	virtual ~WinApplicationWindow() {};

	void init(IRenderContext&) override;

private:
	GLFWwindow* window;
};