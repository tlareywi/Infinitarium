#include "EventSampler.hpp"
#include "../../Engine/Application.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

static std::shared_ptr<IEventSampler> eventSampler = std::make_shared<WindowsEventSampler>();
std::shared_ptr<IApplication> CreateApplication();

void WindowsEventSampler::setTargetWindow( GLFWwindow* window, IRenderContext& c ) {
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

	context = &c;
}

void WindowsEventSampler::onKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods) {
	IEventSampler::Key k;
	k.key = key;
	if (action == GLFW_RELEASE)
		k.state = IEventSampler::State::UP;
	if (action == GLFW_PRESS)
		k.state = IEventSampler::State::DOWN;
	eventSampler->push(k);
}

void WindowsEventSampler::onMouseButton(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseLeftTimer = std::chrono::high_resolution_clock::now();
		leftButtonDown = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		leftButtonDown = false;
		std::chrono::duration<double, std::milli> ms{ std::chrono::high_resolution_clock::now() - mouseLeftTimer };
		if (ms.count() < 200.0) {
			double xpos{ 0 }, ypos{ 0 };
			glfwGetCursorPos(window, &xpos, &ypos);
			IEventSampler::MouseButton mb{ IEventSampler::Button::LEFT, IEventSampler::State::CLICKED, (float)xpos, (float)ypos };
			eventSampler->push(mb);
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		rightButtonDown = true;
		mouseRightTimer = std::chrono::high_resolution_clock::now();
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		rightButtonDown = false;
		std::chrono::duration<double, std::milli> ms{ std::chrono::high_resolution_clock::now() - mouseLeftTimer };
		if (ms.count() < 200.0) {
			double xpos{ 0 }, ypos{ 0 };
			glfwGetCursorPos(window, &xpos, &ypos);
			IEventSampler::MouseButton mb{ IEventSampler::Button::RIGHT, IEventSampler::State::CLICKED, (float)xpos, (float)ypos };
			eventSampler->push(mb);
		}
	}
}

void WindowsEventSampler::onMouseMove(GLFWwindow* window, double xpos, double ypos) {
	if (rightButtonDown) {
		IEventSampler::MouseDrag mm;
		mm.dx = static_cast<float>(xpos - lastX);
		mm.dy = static_cast<float>(ypos - lastY);
		eventSampler->push(mm);
	}

	lastX = xpos;
	lastY = ypos;
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

__declspec(dllexport) std::shared_ptr<IEventSampler> CreateEventSampler() {
	return eventSampler;
}

