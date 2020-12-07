#pragma once

#include "../../Engine/Application.hpp"

#define VK_USE_PLATFORM_WIN32_KHR 
#include "vulkan/vulkan.h"

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_VULKAN

#include <openxr.h>
#include <openxr_platform.h>

class WinApplication : public IApplication {
public:
	static std::shared_ptr<IApplication> Instance();
	WinApplication();
	virtual ~WinApplication() {
		if (ext_xrDestroyDebugUtilsMessengerEXT) {
			ext_xrDestroyDebugUtilsMessengerEXT(debugHook);
		}
	}

	void run() override;
	void stop() override;
	void addManipulator(const std::string& id, float, float, float) override;

	void* platformInstance() override {
		return vkInstance;
	}

	VkInstance getVkInstance();

	XrInstance getXrInstance();

	XrSystemId getXrSystemId() {
		return xrSystemId;
	}

	const VkInstanceCreateInfo& getCreateInfo() {
		return createInfo;
	}

private:
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};


	XrResult CreateVulkanInstanceKHR(XrInstance instance, const XrVulkanInstanceCreateInfoKHR* createInfo, VkInstance* vulkanInstance, VkResult* vulkanResult) {
		PFN_xrCreateVulkanInstanceKHR pfnCreateVulkanInstanceKHR = nullptr;
		xrGetInstanceProcAddr(instance, "xrCreateVulkanInstanceKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfnCreateVulkanInstanceKHR));
		return pfnCreateVulkanInstanceKHR(instance, createInfo, vulkanInstance, vulkanResult);
	}

	bool initValidationLayers( VkInstanceCreateInfo& ); 
	void registerValidationCallBack( VkInstance );
	VkValidationFeaturesEXT enabledFeatures;
	std::vector<VkValidationFeatureEnableEXT> enabledValidationExt;

	static std::shared_ptr<IApplication> instance;
	
	// Vulkan
	VkApplicationInfo appInfo = {};
	VkInstanceCreateInfo createInfo = {};
	std::vector<const char*> extensions;
	VkInstance vkInstance;
	VkDebugUtilsMessengerEXT debugMessenger;
	bool enableValidation;

	// OpenXR
	XrInstance xrInstance;
	XrSystemId xrSystemId;
	PFN_xrCreateDebugUtilsMessengerEXT ext_xrCreateDebugUtilsMessengerEXT = nullptr;
	PFN_xrDestroyDebugUtilsMessengerEXT ext_xrDestroyDebugUtilsMessengerEXT = nullptr;
	XrDebugUtilsMessengerEXT debugHook;
};

