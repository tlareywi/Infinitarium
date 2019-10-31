#pragma once

#include "../../Engine/Texture.hpp"

class VulkanTexture : public ITexture {
public:
	VulkanTexture(glm::uvec2, ITexture::Format) {};
	VulkanTexture(const ITexture& obj) : ITexture(obj) {};

	void prepare(std::shared_ptr<IRenderContext>&) override {};

private:
	//id<MTLTexture> texture;
};

class VulkanRenderTarget : public IRenderTarget {
public:
	VulkanRenderTarget(const IRenderTarget& obj) : IRenderTarget(obj) {}
	VulkanRenderTarget(const glm::uvec2&, Format, Type, Resource) {};
	virtual ~VulkanRenderTarget() {
	}

	//id<MTLTexture> getMetalTexture() {
//		return renderTarget;
//	}

	void prepare(std::shared_ptr<IRenderContext>&) override {};

private:
	//id<MTLTexture> renderTarget = nullptr;
};

