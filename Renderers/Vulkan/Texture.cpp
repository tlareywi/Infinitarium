#include "Texture.hpp"

VulkanRenderTarget::VulkanRenderTarget(const glm::uvec2& d, ITexture::Format f, IRenderTarget::Type t, IRenderTarget::Resource r) :
	IRenderTarget(d, f, t, r) {
	bytesPerRow = 0;
}

__declspec(dllexport) std::shared_ptr<IRenderTarget> CreateRenderTarget(const glm::uvec2& vec, ITexture::Format f, IRenderTarget::Type t, IRenderTarget::Resource r) {
	return std::make_shared<VulkanRenderTarget>(vec, f, t, r);
}

__declspec(dllexport) std::shared_ptr<IRenderTarget> CloneRenderTarget(const IRenderTarget& obj) {
	return std::make_shared<VulkanRenderTarget>(obj);
}

__declspec(dllexport) std::shared_ptr<ITexture> CreateTexture(const glm::uvec2& vec, ITexture::Format f) {
	return std::make_shared<VulkanTexture>(vec, f);
}

__declspec(dllexport) std::shared_ptr<ITexture> CloneTexture(const ITexture& obj) {
	return std::make_shared<VulkanTexture>(obj);
}