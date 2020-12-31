#pragma once

#include "../../Engine/Texture.hpp"
#include "Image.hpp"

class VulkanTexture : public ITexture {
public:
	VulkanTexture(const glm::uvec2& dim, ITexture::Format format) : ITexture(dim, format) {};
	VulkanTexture(const ITexture& obj) : ITexture(obj) {};
	virtual ~VulkanTexture();

	void prepare(IRenderContext&) override;

	void descriptor( VkDescriptorImageInfo& );

private:
	VkFormat getPixelFormat();

	std::shared_ptr<VulkanImage> textureImage{ nullptr };
	VkImageView imageView{ nullptr };
	VkSampler sampler{ nullptr };
	VkDevice vkDevice{ nullptr };
};


