//
//
//

#include "RenderContext.hpp"

VulkanRenderContext::VulkanRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs) : IRenderContext(x, y, w, h, fs) {

}

VulkanRenderContext::VulkanRenderContext(const IRenderContext& obj) : IRenderContext(obj) {

}

VulkanRenderContext::~VulkanRenderContext() {

}

__declspec(dllexport)
	std::shared_ptr<IRenderContext> CreateRenderContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs) {
		std::shared_ptr<IRenderContext> context = std::make_shared<VulkanRenderContext>(x, y, w, h, fs);
		return context;
	}

__declspec(dllexport)
	std::shared_ptr<IRenderContext> CloneRenderContext(const IRenderContext& obj) {
		std::shared_ptr<IRenderContext> context = std::make_shared<VulkanRenderContext>(obj);
		return context;
	}
