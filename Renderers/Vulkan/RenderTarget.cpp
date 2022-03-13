//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "RenderTarget.hpp"
#include "RenderContext.hpp"
#include "RenderPass.hpp"
#include "Image.hpp"

VulkanRenderTarget::VulkanRenderTarget(const glm::uvec2& d, ITexture::Format f, IRenderTarget::Type t, IRenderTarget::Resource r) :
	imageView(nullptr),
	cmdBuffer(nullptr),
	IRenderTarget(d, f, t, r) {
}

VulkanRenderTarget::VulkanRenderTarget( VulkanRenderContext& vkContext, const VkImageViewCreateInfo& createInfo ) {
	device = vkContext.getVulkanDevice();

	// Swapchain target constructor
	imageView = std::make_shared<ImageViewResource>(device, createInfo);

	framebufferAttachments.push_back((*imageView)());

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	CheckVkResult(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinished));

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vkContext.getCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	cmdBuffer = std::make_shared<CommandBufferResource>(vkContext.getVulkanDevice(), allocInfo);
}

VulkanRenderTarget::VulkanRenderTarget(VulkanRenderContext& vkContext, const VulkanRenderTarget& sourceTarget) {
	initOffScreen(vkContext, sourceTarget);
}

void VulkanRenderTarget::initOffScreen( VulkanRenderContext& vkContext, const VulkanRenderTarget& sourceTarget ) {
	if (device) return; // Already inited

	// Offscreen target constructor
	device = vkContext.getVulkanDevice();

	setExtent(sourceTarget.dim.x, sourceTarget.dim.y);

	// Image Resource
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(sourceTarget.dim.x);
	imageInfo.extent.height = static_cast<uint32_t>(sourceTarget.dim.y);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = sourceTarget.getPixelFormat();
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = static_cast<VkSampleCountFlagBits>(multisampleLevel);
	imageInfo.flags = 0;
	backingImage = std::make_shared<VulkanImage>(vkContext, imageInfo);

	// ImageView
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = backingImage->getImage();
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = sourceTarget.getPixelFormat();
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	imageView = std::make_shared<ImageViewResource>(device, createInfo);

	// Sampler to access resource on multi-pass pipeline
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1.0; // TODO: Need to query device caps for this.
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture sampler!");

	framebufferAttachments.push_back( (*imageView)() );

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	CheckVkResult(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinished));

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vkContext.getCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	cmdBuffer = std::make_shared<CommandBufferResource>(vkContext.getVulkanDevice(), allocInfo);
}

VulkanRenderTarget::~VulkanRenderTarget() {
	framebufferAttachments.clear();

	if (device) {
		vkDestroySemaphore(device, renderFinished, nullptr);
		if( sampler )
			vkDestroySampler(device, sampler, nullptr);
	}
	renderPassAttachments.clear();
	imageView = nullptr;
	cmdBuffer = nullptr;
}

// TODO: Refactor. Both RenderTarget and Texture have similar use of samplers ... and imageviews.
void VulkanRenderTarget::descriptor( VkDescriptorImageInfo& descriptor, VulkanRenderContext& vkContext ) {
	// If the render target is being used by an IRenderable then we may get here before the render pass that draws to the target. 
	// Make sure the resouces exist. 
	initOffScreen(vkContext, *this);
	descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	descriptor.imageView = (*imageView)();
	descriptor.sampler = sampler;
}

VkFormat VulkanRenderTarget::getPixelFormat() const {
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

std::shared_ptr<FramebufferResource> VulkanRenderTarget::getFramebuffer( const VulkanRenderPass& rp ) {
	return renderPassAttachments[reinterpret_cast<unsigned long long>(&rp)];
}

VkFence VulkanRenderTarget::getFence() {
	if (cmdBuffer == nullptr)
		return VK_NULL_HANDLE;
	else
		return cmdBuffer->getFence();
}

VkSemaphore VulkanRenderTarget::getSemaphore() {
	return renderFinished;
}

const std::vector<VkImageView>& VulkanRenderTarget::getFramebufferAttachments() const {

	return framebufferAttachments;
}

void VulkanRenderTarget::attach(VulkanRenderContext& ctx, VulkanRenderPass& pass) {
	auto itr{ renderPassAttachments.find(reinterpret_cast<unsigned long long>(&pass)) };
	if (itr != renderPassAttachments.end())
		renderPassAttachments.erase(itr);

	std::vector<VkImageView> compositeAttachments(framebufferAttachments.begin(), framebufferAttachments.end());
	for (auto& attachment : pass.getFramebufferAttachments()) {
		bool hit = false;
		for (auto& local : compositeAttachments) {
			if (local == attachment)
				hit = true;
		}

		if (!hit)
			compositeAttachments.push_back(attachment);
	}

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = pass.getVulkanRenderPass();
	framebufferInfo.attachmentCount = static_cast<uint32_t>(compositeAttachments.size());
	framebufferInfo.pAttachments = compositeAttachments.data();
	framebufferInfo.width = dim.x;
	framebufferInfo.height = dim.y;
	framebufferInfo.layers = 1;

	VulkanRenderContext& vkCtx{ dynamic_cast<VulkanRenderContext&>( ctx ) };
	renderPassAttachments[reinterpret_cast<unsigned long long>(&pass)] = std::make_shared<FramebufferResource>(vkCtx.getVulkanDevice(), framebufferInfo);
}

RenderTargetStack::RenderTargetStack(VulkanRenderContext& ctx, const std::vector<VkImageViewCreateInfo>& createInfos) {
	renderTargets.reserve(createInfos.size());
	for( auto& createInfo : createInfos ) {
		renderTargets.push_back( std::make_shared<VulkanRenderTarget>(ctx, createInfo) );
	}
}

RenderTargetStack::RenderTargetStack(VulkanRenderContext& vkContext, const VulkanRenderTarget& renderTarget, uint8_t copies) {
	renderTargets.reserve(copies);
	while (copies--) {
		renderTargets.push_back(std::make_shared<VulkanRenderTarget>(vkContext, renderTarget));
	}
}

RenderTargetStack::~RenderTargetStack() {
	renderTargets.clear();
}

VulkanRenderTarget& RenderTargetStack::operator[](uint8_t indx) {
	return *renderTargets[indx];
}

void RenderTargetStack::attach(IRenderContext& ctx, IRenderPass& rp, unsigned int w, unsigned int h) {
	VulkanRenderContext& vkCtx{ dynamic_cast<VulkanRenderContext&>(ctx) };
	VulkanRenderPass& vkRp{ dynamic_cast<VulkanRenderPass&>(rp) };

	for (auto& target : renderTargets) {
		if( w && h )
			target->setExtent(w, h);

		target->attach(vkCtx, vkRp);
	}
}

RENDERER_EXPORT std::shared_ptr<IRenderTarget> CreateRenderTarget(const glm::uvec2& vec, ITexture::Format f, IRenderTarget::Type t, IRenderTarget::Resource r) {
	return std::make_shared<VulkanRenderTarget>(vec, f, t, r);
}

RENDERER_EXPORT std::shared_ptr<IRenderTarget> CloneRenderTarget(const IRenderTarget& obj) {
	return std::make_shared<VulkanRenderTarget>(obj);
}