#include "Application.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

std::shared_ptr<IApplication> WinApplication::instance{ nullptr };

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		std::string type("General warning");
		if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
			type = "API violation";
		else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
			type = "Performance";

		std::cout << type << ": validation layer: " << pCallbackData->pMessage << std::endl;
	}

	return VK_FALSE;
}

std::shared_ptr<IApplication> WinApplication::Instance() {
	if (!instance)
		instance = std::make_shared<WinApplication>();

	return instance;
}

WinApplication::WinApplication() {
	glfwInit();

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Stargazer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Infinitarium";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );
	createInfo.enabledLayerCount = 0;

//#ifdef NDEBUG
//	enableValidation{ false };
//#else
//	enableValidation = initValidationLayers(createInfo);
//	if( enableValidation )
//		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//#endif

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

    if( vkCreateInstance( &createInfo, nullptr, &vkInstance) != VK_SUCCESS )
		throw std::runtime_error("Failed to initialize Vulkan runtime.");

	if (enableValidation)
		registerValidationCallBack( vkInstance );
}

void WinApplication::run() {
	while( 1 ) {
		glfwPollEvents();
	}
}

void WinApplication::stop() {
	if (enableValidation) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( vkInstance, "vkDestroyDebugUtilsMessengerEXT" );
		if (func != nullptr) {
			func(vkInstance, debugMessenger, nullptr);
		}
	}

	vkDestroyInstance( vkInstance, nullptr );

	glfwTerminate();
}

void WinApplication::addManipulator(const std::string& id, float, float, float) {

}

bool WinApplication::initValidationLayers( VkInstanceCreateInfo& createInfo ) {
	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			std::cout << "Warning: unable to initialize requested Vulkan validation layer(s)." << std::endl;
			return false;
		}
	}

	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();

	return true;
}

void WinApplication::registerValidationCallBack( VkInstance vkInstance ) {
	// Setup callback
	VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
	debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugInfo.pfnUserCallback = debugCallback;
	debugInfo.pUserData = nullptr; // Optional

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
	if( func ) {
		if( func(vkInstance, &debugInfo, nullptr, &debugMessenger) != VK_SUCCESS )
			std::cout << "Warning: unable to register Vulkan validation callback." << std::endl;;
	}
}

__declspec(dllexport) std::shared_ptr<IApplication> CreateApplication() {
	return WinApplication::Instance();
}
