#pragma once

#include "../../Engine/Application.hpp"

#define VK_USE_PLATFORM_WIN32_KHR 
#include "vulkan/vulkan.h"

class WinApplication : public IApplication {
public:
	static std::shared_ptr<IApplication> Instance();
	WinApplication();
	virtual ~WinApplication() {}

	void run() override;
	void stop() override;
	void addManipulator(const std::string& id, float, float, float) override;

	void* platformInstance() override {
		return vkInstance;
	}

	VkInstance getVkInstance() {
		return vkInstance;
	}

private:
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	bool initValidationLayers( VkInstanceCreateInfo& ); 
	void registerValidationCallBack( VkInstance );

	static std::shared_ptr<IApplication> instance;
	
	VkInstance vkInstance;
	VkDebugUtilsMessengerEXT debugMessenger;
	bool enableValidation;
};