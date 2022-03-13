//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "../../Engine/IImGUI.hpp"
#include "examples/imgui_impl_vulkan.h"
#include "examples/imgui_impl_glfw.h"

#include <functional>

class VulkanImGUI : public IImGUI {
public:
	VulkanImGUI() {
		std::cout << "CreateImGUI Constructor" << std::endl;
	}
	virtual ~VulkanImGUI();

	void prepare(IRenderContext&) override;
	void apply(IRenderPass&) override;

	void update() override;
	void render(IRenderPass& renderPass) override;

private:
	void initImGUI(IRenderPass&);

	ImGui_ImplVulkan_InitInfo init_info = {};

	GLFWwindow* window{ nullptr };
	VkCommandBuffer commandBuffer{ nullptr };
	std::function<void()> initializeFonts;

	int fbWidth;
	int fbHeight;
};