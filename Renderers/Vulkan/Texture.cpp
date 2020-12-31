#include "Texture.hpp"
#include "RenderContext.hpp"

VulkanTexture::~VulkanTexture() {
	textureImage = nullptr;
	if (vkDevice) {
		vkDestroySampler(vkDevice, sampler, nullptr);
		vkDestroyImageView(vkDevice, imageView, nullptr);
	}
}

VkFormat VulkanTexture::getPixelFormat() {
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
		return VK_FORMAT_R8G8B8A8_SRGB;
	default:
		std::cout << "Warning: requested pixel format for VulkanTexture not implemented. Using R8G8B8A8_SRGB." << std::endl;
		return VK_FORMAT_R8G8B8A8_SRGB;
	}
}

void VulkanTexture::descriptor(VkDescriptorImageInfo& descriptor) {
	descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	descriptor.imageView = imageView;
	descriptor.sampler = sampler;
}

void VulkanTexture::prepare(IRenderContext& context) {
	VulkanRenderContext& vkContext{ dynamic_cast<VulkanRenderContext&>(context) };
	vkDevice = vkContext.getVulkanDevice();

	// Create VulkanImage
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(dim.x);
	imageInfo.extent.height = static_cast<uint32_t>(dim.y);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = getPixelFormat();
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;
	textureImage = std::make_shared<VulkanImage>(vkContext, imageInfo);

	// Stage data buffer
	std::shared_ptr<VulkanBuffer> imgData = std::make_shared<VulkanBuffer>(vkContext);
	imgData->set(image);

	// Copy buffer to VulkanImage
	VkCommandBuffer commandBuffer = vkContext.allocTransientBuffer();
	textureImage->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	glm::ivec4 rect{ 0,0,dim.x,dim.y };
	textureImage->copy(commandBuffer, *imgData, rect);
	textureImage->transitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	vkContext.submitTransientBuffer(commandBuffer);

	// Construct ImageView
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = textureImage->getImage();
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = imageInfo.format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	if (vkCreateImageView(vkDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture image view!");

	// Init sampler
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
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
	if (vkCreateSampler(vkDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture sampler!");
};

__declspec(dllexport) std::shared_ptr<ITexture> CreateTexture(const glm::uvec2& vec, ITexture::Format f) {
	return std::make_shared<VulkanTexture>(vec, f);
}

__declspec(dllexport) std::shared_ptr<ITexture> CloneTexture(const ITexture& obj) {
	return std::make_shared<VulkanTexture>(obj);
}