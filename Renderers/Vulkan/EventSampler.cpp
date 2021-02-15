#include "EventSampler.hpp"
#include "../../Engine/Application.hpp"
#include "imgui.h"

#if (WIN32)
	#define GLFW_EXPOSE_NATIVE_WIN32
#else
	#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>

static std::shared_ptr<IEventSampler> eventSampler = std::make_shared<WindowsEventSampler>();
std::shared_ptr<IApplication> CreateApplication();

void WindowsEventSampler::setTargetWindow(GLFWwindow* window, IRenderContext& c) {
	glfwSetWindowUserPointer(window, this);

	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		WindowsEventSampler* sampler = static_cast<WindowsEventSampler*>(glfwGetWindowUserPointer(window));
		sampler->onMouseButton(window, button, action, mods);
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		WindowsEventSampler* sampler = static_cast<WindowsEventSampler*>(glfwGetWindowUserPointer(window));
		sampler->onMouseMove(window, xpos, ypos);
	});

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h) {
		WindowsEventSampler* sampler = static_cast<WindowsEventSampler*>(glfwGetWindowUserPointer(window));
		sampler->onFramebufferSize(window, w, h);
	});

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		WindowsEventSampler* sampler = static_cast<WindowsEventSampler*>(glfwGetWindowUserPointer(window));
		sampler->onKeyDown(window, key, scancode, action, mods);
	});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		WindowsEventSampler* sampler = static_cast<WindowsEventSampler*>(glfwGetWindowUserPointer(window));
		sampler->onScroll(window, xoffset, yoffset);
	});

	context = &c;
}

void WindowsEventSampler::onKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods) {
	const char* name = glfwGetKeyName(key, scancode);
	IEventSampler::Key k;
	if (name)
		k.key = name[0];
	else
		k.key = key;
	if (action == GLFW_RELEASE)
		k.state = IEventSampler::State::UP;
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
		k.state = IEventSampler::State::DOWN;
	eventSampler->push(k);
}

void WindowsEventSampler::onMouseButton(GLFWwindow* window, int button, int action, int mods) {
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	if( action == GLFW_PRESS )
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	else if (action == GLFW_RELEASE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseLeftTimer = std::chrono::high_resolution_clock::now();
		leftButtonDown = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		leftButtonDown = false;
		std::chrono::duration<double, std::milli> ms{ std::chrono::high_resolution_clock::now() - mouseLeftTimer };
		std::chrono::duration<double, std::milli> doubleTime{ std::chrono::high_resolution_clock::now() - mouseDoubleTimer };

		if (doubleTime.count() < 500.0) {
			double xpos{ 0 }, ypos{ 0 };
			glfwGetCursorPos(window, &xpos, &ypos);
			IEventSampler::MouseButton mb{ IEventSampler::Button::LEFT, IEventSampler::State::DBL_CLICKED, (float)xpos, (float)ypos };
			eventSampler->push(mb);
		}
		else if (ms.count() < 200.0) {
			double xpos{ 0 }, ypos{ 0 };
			glfwGetCursorPos(window, &xpos, &ypos);
			IEventSampler::MouseButton mb{ IEventSampler::Button::LEFT, IEventSampler::State::CLICKED, (float)xpos, (float)ypos };
			eventSampler->push(mb);
			mouseDoubleTimer = std::chrono::high_resolution_clock::now();
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		rightButtonDown = true;
		mouseRightTimer = std::chrono::high_resolution_clock::now();
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		rightButtonDown = false;
		std::chrono::duration<double, std::milli> ms{ std::chrono::high_resolution_clock::now() - mouseRightTimer };
		if (ms.count() < 200.0) {
			double xpos{ 0 }, ypos{ 0 };
			glfwGetCursorPos(window, &xpos, &ypos);
			IEventSampler::MouseButton mb{ IEventSampler::Button::RIGHT, IEventSampler::State::CLICKED, (float)xpos, (float)ypos };
			eventSampler->push(mb);
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		middleButtonDown = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
		middleButtonDown = false;
	}
}

void WindowsEventSampler::onMouseMove(GLFWwindow* window, double xpos, double ypos) {
	if (rightButtonDown) {
		IEventSampler::MouseDrag mm;
		mm.button = IEventSampler::Button::RIGHT;
		mm.dx = static_cast<float>(xpos - lastX);
		mm.dy = static_cast<float>(ypos - lastY);
		eventSampler->push(mm);
	}
	else if (leftButtonDown) {
		IEventSampler::MouseDrag mm;
		mm.button = IEventSampler::Button::LEFT;
		mm.dx = static_cast<float>(xpos - lastX);
		mm.dy = static_cast<float>(ypos - lastY);
		eventSampler->push(mm);
	}
	else if (middleButtonDown) {
		IEventSampler::MouseDrag mm;
		mm.button = IEventSampler::Button::MIDDLE;
		mm.dx = static_cast<float>(xpos - lastX);
		mm.dy = static_cast<float>(ypos - lastY);
		eventSampler->push(mm);
	}

	lastX = xpos;
	lastY = ypos;
}

void WindowsEventSampler::onScroll(GLFWwindow* window, double xoffset, double yoffset) {
	// For scroll wheel events yoffset is just either 1 or -1.
	IEventSampler::MouseMove mm;
	mm.dz = static_cast<float>(yoffset);
	eventSampler->push(mm);
}

void WindowsEventSampler::onFramebufferSize(GLFWwindow* window, int w, int h) {
	if (w == 0 && h == 0) {
		context->pauseRendering(true);
	}
	else {
		std::tuple<IRenderContext&> args(*context);
		Event e(args);
		e.setName("resetScene");
		CreateApplication()->invoke(e);

		context->pauseRendering(false);
	}
}

RENDERER_EXPORT std::shared_ptr<IEventSampler> CreateEventSampler() {
	return eventSampler;
}

