//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "../../Engine/Application.hpp"

#include <atomic>

#if defined(WIN32)
	#define VK_USE_PLATFORM_WIN32_KHR 
#elif defined(__APPLE__)
    #define VK_USE_PLATFORM_METAL_EXT
#else
	#define VK_USE_PLATFORM_X11_KHR
#endif

#include "vulkan/vulkan.h"

#if (USE_OPENXR)
	#define XR_USE_PLATFORM_WIN32
	#define XR_USE_GRAPHICS_API_VULKAN
	#include <openxr.h>
	#include <openxr_platform.h>
#else
	using XrInstance = VkInstance;
	using XrSystemId = uint32_t;
#endif

class WinApplication : public IApplication {
public:
	static std::shared_ptr<IApplication> Instance();
	WinApplication();
	virtual ~WinApplication();
    
    void runOnUIThread( std::function<void()>& f ) override { f(); }

	void platformRun() override;
	void stop() override;
	void destroy() override {
		instance = nullptr;
	}
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
    
protected:
    static std::shared_ptr<IApplication> instance;

private:
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	bool initValidationLayers( VkInstanceCreateInfo& ); 
	void registerValidationCallBack( VkInstance );
	VkValidationFeaturesEXT enabledFeatures;
	std::vector<VkValidationFeatureEnableEXT> enabledValidationExt;
	
	// Vulkan
	VkApplicationInfo appInfo = {};
	VkInstanceCreateInfo createInfo = {};
	std::vector<const char*> extensions;
	VkInstance vkInstance;
	VkDebugUtilsMessengerEXT debugMessenger;
	bool enableValidation;

	XrInstance xrInstance;
	XrSystemId xrSystemId;
#if (USE_OPENXR)
	// OpenXR
	PFN_xrCreateDebugUtilsMessengerEXT ext_xrCreateDebugUtilsMessengerEXT = nullptr;
	PFN_xrDestroyDebugUtilsMessengerEXT ext_xrDestroyDebugUtilsMessengerEXT = nullptr;
	XrDebugUtilsMessengerEXT debugHook;
	XrResult CreateVulkanInstanceKHR(XrInstance instance, const XrVulkanInstanceCreateInfoKHR* createInfo, VkInstance* vulkanInstance, VkResult* vulkanResult) {
		PFN_xrCreateVulkanInstanceKHR pfnCreateVulkanInstanceKHR = nullptr;
		xrGetInstanceProcAddr(instance, "xrCreateVulkanInstanceKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfnCreateVulkanInstanceKHR));
		return pfnCreateVulkanInstanceKHR(instance, createInfo, vulkanInstance, vulkanResult);
	}
#endif

};

class MacApplication : public WinApplication {
public:
    static std::shared_ptr<IApplication> Instance();
    void runOnUIThread( std::function<void()>& ) override;
};

// Forward Decl
RENDERER_EXPORT std::shared_ptr<IApplication> CreateApplication();

