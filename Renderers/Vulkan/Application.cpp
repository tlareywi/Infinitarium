#include "Application.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <thread>
#include <algorithm>

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

WinApplication::WinApplication() : vkInstance(nullptr), xrInstance(nullptr) {
	glfwInit();

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Stargazer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Infinitarium";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExt = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (unsigned int i = 0; i < glfwExtensionCount; ++i)
		extensions.push_back(glfwExt[i]);
	createInfo.enabledLayerCount = 0;

#ifdef NDEBUG
	enableValidation = false;
#else
	enableValidation = initValidationLayers(createInfo);
	if( enableValidation )
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	createInfo.enabledExtensionCount = (uint32_t)extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();
}

VkInstance WinApplication::getVkInstance() {
	if (!vkInstance) {
		if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS)
			throw std::runtime_error("Failed to initialize Vulkan runtime.");

		if (enableValidation)
			registerValidationCallBack(vkInstance);
	}

	return vkInstance;
}

XrInstance WinApplication::getXrInstance() {
	if (xrInstance) return xrInstance;

	std::vector<const char*> useExtensions;
	const char* askExtensions[] = {
		XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME,
		XR_EXT_DEBUG_UTILS_EXTENSION_NAME,
	};

	uint32_t extCount{ 0 };
	xrEnumerateInstanceExtensionProperties(nullptr, 0, &extCount, nullptr);
	std::vector<XrExtensionProperties> xr_exts(extCount, { XR_TYPE_EXTENSION_PROPERTIES });
	xrEnumerateInstanceExtensionProperties(nullptr, extCount, &extCount, xr_exts.data());

	for (size_t i = 0; i < xr_exts.size(); i++) {
		for (int32_t ask = 0; ask < _countof(askExtensions); ask++) {
			if (strcmp(askExtensions[ask], xr_exts[i].extensionName) == 0) {
				useExtensions.push_back(askExtensions[ask]);
				break;
			}
		}
	}

	if (!std::any_of(useExtensions.begin(), useExtensions.end(), [](const char* ext) {
		return strcmp(ext, XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME) == 0;
	})) {
		throw std::runtime_error("Fatal: XR device does not support Vulkan renderer!");
	}

	{
		XrInstanceCreateInfo createInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
		createInfo.enabledExtensionCount = (uint32_t)useExtensions.size();
		createInfo.enabledExtensionNames = useExtensions.data();
		createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
		strcpy_s(createInfo.applicationInfo.applicationName, "Infinitarium");
		xrCreateInstance(&createInfo, &xrInstance);
	}

	// Request a form factor from the device (HMD, Handheld, etc.)
	XrSystemGetInfo systemInfo = { XR_TYPE_SYSTEM_GET_INFO };
	systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	if( xrGetSystem(xrInstance, &systemInfo, &xrSystemId) != VK_SUCCESS )
		throw std::runtime_error("Failed to enumerate head mounted display. Is device attached?");

	XrVulkanInstanceCreateInfoKHR xrCreateInfo{ XR_TYPE_VULKAN_INSTANCE_CREATE_INFO_KHR };
	xrCreateInfo.systemId = xrSystemId;
	xrCreateInfo.pfnGetInstanceProcAddr = &vkGetInstanceProcAddr;
	xrCreateInfo.vulkanCreateInfo = &createInfo;
	xrCreateInfo.vulkanAllocator = nullptr;

	VkResult vkResult;

	if (CreateVulkanInstanceKHR(xrInstance, &xrCreateInfo, &vkInstance, &vkResult) != XR_SUCCESS || vkResult != VK_SUCCESS)
		throw std::runtime_error("Failed to initialize Vulkan XR runtime.");

	if (enableValidation)
		registerValidationCallBack(vkInstance);

	// Hook up debug callback
	xrGetInstanceProcAddr(xrInstance, "xrCreateDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)(&ext_xrCreateDebugUtilsMessengerEXT));
	xrGetInstanceProcAddr(xrInstance, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)(&ext_xrDestroyDebugUtilsMessengerEXT));

	XrDebugUtilsMessengerCreateInfoEXT debugInfo = { XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
	debugInfo.messageTypes =
		XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
	debugInfo.messageSeverities =
		//	XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugInfo.userCallback = [](XrDebugUtilsMessageSeverityFlagsEXT severity, XrDebugUtilsMessageTypeFlagsEXT types, const XrDebugUtilsMessengerCallbackDataEXT* msg, void* user_data) {
		std::cout << msg->functionName << ": " << msg->message << std::endl;
		return (XrBool32)XR_FALSE;
	};

	if (ext_xrCreateDebugUtilsMessengerEXT)
		ext_xrCreateDebugUtilsMessengerEXT(xrInstance, &debugInfo, &debugHook);

	return xrInstance;
}

void WinApplication::run() {
	while( 1 ) {
		glfwPollEvents();
		std::this_thread::yield();
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

	enabledValidationExt.push_back(VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT);
	enabledFeatures.pDisabledValidationFeatures = nullptr;
	enabledFeatures.disabledValidationFeatureCount = 0;
	enabledFeatures.pEnabledValidationFeatures = enabledValidationExt.data();
	enabledFeatures.enabledValidationFeatureCount = static_cast<uint32_t>(enabledValidationExt.size());
	enabledFeatures.pNext = nullptr;

	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();
	createInfo.pNext = &enabledFeatures;

	return true;
}

void WinApplication::registerValidationCallBack( VkInstance vkInstance ) {
	// Setup callback
	VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
	debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
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

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID ) {
	return TRUE;
}

