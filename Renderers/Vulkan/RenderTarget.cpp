#include "RenderTarget.hpp"
#include "RenderContext.hpp"
#include "RenderPass.hpp"

VulkanRenderTarget::VulkanRenderTarget(const glm::uvec2& d, ITexture::Format f, IRenderTarget::Type t, IRenderTarget::Resource r) :
	colorView(nullptr),
	depthView(nullptr),
	framebuffer(nullptr),
	cmdBuffer(nullptr),
	IRenderTarget(d, f, t, r) {
}

VulkanRenderTarget::VulkanRenderTarget( VkDevice logicalDevice, VkImageViewCreateInfo& createInfo ) {
	// Swapchain target constructor
	colorView = std::make_shared<ImageViewResource>(logicalDevice, createInfo);
}

VulkanRenderTarget::~VulkanRenderTarget() {

}

VkFormat VulkanRenderTarget::getPixelFormat() {
	switch (format) {
	case BRGA8:
		return VK_FORMAT_B8G8R8A8_UNORM;
	case RGBA8:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case RU32:
		return VK_FORMAT_R32_UINT;
	case RF32:
		return VK_FORMAT_R32_SFLOAT;
	case RGBA8_sRGB:
	default:
		return VK_FORMAT_R8G8B8A8_SRGB;
	}
}

VkCommandBuffer VulkanRenderTarget::getCmdBuffer() {
	if (cmdBuffer == nullptr)
		return VK_NULL_HANDLE;
	else
		return (*cmdBuffer)();
}

VkFramebuffer VulkanRenderTarget::getFramebuffer() {
	if (framebuffer == nullptr)
		return VK_NULL_HANDLE;
	else
		return (*framebuffer)();
}

void VulkanRenderTarget::attach(IRenderContext& context, IRenderPass& renderPass) {
	if (!colorView) return;

	VulkanRenderContext& vkContext{ dynamic_cast<VulkanRenderContext&>(context) };
	VulkanRenderPass& vkRenderPass{ dynamic_cast<VulkanRenderPass&>(renderPass) };

	VkImageView attachments[] = {
		// Additional here would presumably be stuff like depth and stencil.
		(*colorView)()
	};

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = vkRenderPass.getVulkanRenderPass();
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = attachments;
	framebufferInfo.width = dim.x;
	framebufferInfo.height = dim.y;
	framebufferInfo.layers = 1;

	framebuffer = std::make_shared<FramebufferResource>(vkContext.getVulkanDevice(), framebufferInfo);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vkContext.getCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	cmdBuffer = std::make_shared<CommandBufferResource>(vkContext.getVulkanDevice(), allocInfo);
}

__declspec(dllexport) std::shared_ptr<IRenderTarget> CreateRenderTarget(const glm::uvec2& vec, ITexture::Format f, IRenderTarget::Type t, IRenderTarget::Resource r) {
	return std::make_shared<VulkanRenderTarget>(vec, f, t, r);
}

__declspec(dllexport) std::shared_ptr<IRenderTarget> CloneRenderTarget(const IRenderTarget& obj) {
	return std::make_shared<VulkanRenderTarget>(obj);
}