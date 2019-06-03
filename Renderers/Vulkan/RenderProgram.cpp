#include "RenderProgram.hpp"
#include "UniformMap.h"
#include "RenderContext.hpp"

#include "../../config.h"

#include <sstream>
#include <fstream>

VulkanRenderProgram::~VulkanRenderProgram() {
	if (device) {
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}
}

void VulkanRenderProgram::prepare(IRenderState& state) {
	/*vertex = [program newFunctionWithName : @"vertexShader"];
	fragment = [program newFunctionWithName : @"fragmentShader"];

	MetalRenderState* metalState = dynamic_cast<MetalRenderState*>(&state);
	metalState->getPipelineDescriptor().vertexFunction = vertex;
	metalState->getPipelineDescriptor().fragmentFunction = fragment; */
}

void VulkanRenderProgram::apply(IRenderState& state) {

}

void VulkanRenderProgram::compile(const std::string& name, IRenderContext& context) {
	VulkanRenderContext* c = dynamic_cast<VulkanRenderContext*>(&context);
	device = c->getVulkanDevice();

	std::string vertPath{ std::string(INSTALL_ROOT) + std::string("/share/Infinitarium/shaders/glsl/") + name + ".glsl.vert" };
	std::string fragPath{ std::string(INSTALL_ROOT) + std::string("/share/Infinitarium/shaders/glsl/") + name + ".glsl.frag" };

	std::ifstream vertFile(vertPath, std::ifstream::in);
	std::ifstream fragFile(fragPath, std::ifstream::in);
	if (vertFile.fail() || fragFile.fail()) {
		std::cerr << "Error locating " << name << std::endl;
		return;
	}
	
	{
		std::stringstream buffer;
		buffer << "#version 450\n" << uniformBlock << vertFile.rdbuf();
		std::vector<uint32_t> code{ compileToByteCode(name, shaderc_glsl_default_vertex_shader, buffer.str(), true) };
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = code.data();
		if (vkCreateShaderModule(device, &createInfo, nullptr, &vertShaderModule) != VK_SUCCESS) {
			std::cerr << "Error compiling " << name << std::endl;
			return;
		}
	}
	{
		std::stringstream buffer;
		buffer << "#version 450\n#extension GL_ARB_separate_shader_objects : enable\n" << uniformBlock << fragFile.rdbuf();
		std::vector<uint32_t> code{ compileToByteCode(name, shaderc_glsl_default_fragment_shader, buffer.str(), true) };
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = code.data();
		if (vkCreateShaderModule(device, &createInfo, nullptr, &fragShaderModule) != VK_SUCCESS) {
			std::cerr << "Error compiling " << name << std::endl;
			return;
		}
	}

	vertFile.close();
	fragFile.close();

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
}

void VulkanRenderProgram::injectUniformStruct(const std::vector<std::pair<std::string, UniformType>>& uniforms) {
	std::stringstream ss;
	ss << "\n\nuniform ConstUniforms { \n";

	for (auto& i : uniforms)
		ss << i.second << " " << i.first << ";\n";

	ss << "};\n\n";

	uniformBlock = ss.str();
}

// Compiles a shader to a SPIR-V binary. Returns the binary as a vector of 32-bit words.
std::vector<uint32_t> VulkanRenderProgram::compileToByteCode(const std::string& source_name, shaderc_shader_kind kind, const std::string& source, bool optimize) {

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	// Like -DMY_DEFINE=1
	//options.AddMacroDefinition("MY_DEFINE", "1");
	if (optimize) 
		options.SetOptimizationLevel(shaderc_optimization_level_size);

	shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, kind, source_name.c_str(), options);

	if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << module.GetErrorMessage();
		return std::vector<uint32_t>();
	}

	return { module.cbegin(), module.cend() };
}

__declspec(dllexport)
	std::shared_ptr<IRenderProgram> CreateRenderProgram() {
		return std::make_shared<VulkanRenderProgram>();
	}
