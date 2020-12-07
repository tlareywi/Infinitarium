#pragma once

#include "../../Engine/Texture.hpp"

class VulkanTexture : public ITexture {
public:
	VulkanTexture(glm::uvec2, ITexture::Format) {};
	VulkanTexture(const ITexture& obj) : ITexture(obj) {};

	void prepare(IRenderContext&) override {};

private:
	//id<MTLTexture> texture;
};


