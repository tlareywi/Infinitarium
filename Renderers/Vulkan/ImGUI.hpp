#pragma once

#include "../../Engine/IImGUI.hpp"
#include "examples/imgui_impl_vulkan.h"

class VulkanImGUI : public IImGUI {
public:
	VulkanImGUI() {}
	virtual ~VulkanImGUI();

	void prepare(IRenderContext&) override;
	void apply(IRenderPass&) override;

private:
	void initImGUI(IRenderPass&);
	bool dirty{ false };

	ImGui_ImplVulkan_InitInfo init_info = {};
};