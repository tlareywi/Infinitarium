#include "Texture.hpp"

__declspec(dllexport) std::shared_ptr<ITexture> CreateTexture(const glm::uvec2& vec, ITexture::Format f) {
	return std::make_shared<VulkanTexture>(vec, f);
}

__declspec(dllexport) std::shared_ptr<ITexture> CloneTexture(const ITexture& obj) {
	return std::make_shared<VulkanTexture>(obj);
}