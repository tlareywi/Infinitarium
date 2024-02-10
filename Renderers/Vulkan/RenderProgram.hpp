//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "../../Engine/RenderProgram.hpp"

#include "vulkan/vulkan.h"

#include <shaderc/shaderc.hpp>

///
/// brief Metal implementation of RenderProgram
///
class VulkanRenderProgram : public IRenderProgram {
public:
	VulkanRenderProgram() : device(nullptr), vertShaderModule(VK_NULL_HANDLE), fragShaderModule(VK_NULL_HANDLE) {}
	virtual ~VulkanRenderProgram();

	void prepare(IRenderState& state) override;

	void apply(IRenderState& state) override;

	void compile(const std::string& name, IRenderContext& context) override;

	void injectUniformStruct(const std::vector<std::pair<std::string, Uniform>>& uniforms) override;

private:
	std::vector<uint32_t> compileToByteCode(const std::string& source_name, shaderc_shader_kind kind, const std::string& source, bool optimize = false);

	VkDevice device;

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

	VkPipelineShaderStageCreateInfo shaderStages[2];

	std::string uniformBlock;
};
