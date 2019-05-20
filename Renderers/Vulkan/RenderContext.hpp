#pragma once

#include "../../Engine/RenderContext.hpp"

#include "vulkan/vulkan.h"

///
///
///
class VulkanRenderContext : public IRenderContext {
public:
	VulkanRenderContext(const IRenderContext& obj);
	VulkanRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs);
	virtual ~VulkanRenderContext();

private:
	VkInstance instance;
};

