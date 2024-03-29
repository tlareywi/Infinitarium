//
//  Copyright � 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "ImGUI.hpp"
#include "RenderContext.hpp"
#include "RenderPass.hpp"
#include "Application.hpp"

#include <thread>

VulkanImGUI::~VulkanImGUI() {
	ImGui_ImplVulkan_Shutdown();

	if( window )
		ImGui_ImplGlfw_Shutdown();
}

void VulkanImGUI::initImGUI(IRenderPass& renderPass) {
	if (!init_info.CheckVkResultFn) { // First run
		ImGuiIO& io = ImGui::GetIO();
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
        if( window ) {
            std::atomic<bool> done{false};
            std::function<void()> f = [this, &done](){
                ImGui_ImplGlfw_InitForVulkan(window, true);
                done = true;
            };
            CreateApplication()->runOnUIThread( f );
            while( !done )
                std::this_thread::yield();
        }
		else {
			io.DisplaySize.x = static_cast<float>(fbWidth);
			io.DisplaySize.y = static_cast<float>(fbHeight);
		}

		init_info.CheckVkResultFn = [](VkResult result) {
			assert(result == VK_SUCCESS);
		};
	}
	else
		ImGui_ImplVulkan_Shutdown();

	VulkanRenderPass& vkRenderPass{ dynamic_cast<VulkanRenderPass&>(renderPass) };
	if( !ImGui_ImplVulkan_Init(&init_info, vkRenderPass.getVulkanRenderPass()) )
		throw("Failed to initialize ImGUI.");

	initializeFonts();
}

void VulkanImGUI::prepare(IRenderContext& context) {
	VulkanRenderContext& vkContext{ dynamic_cast<VulkanRenderContext&>(context) };
	VkSwapchainCreateInfoKHR swapchainInfo;
	vkContext.getVulkanSwapchainInfo(swapchainInfo);

	init_info.Instance = vkContext.getVkInstance();
	init_info.PhysicalDevice = vkContext.getPhysicalDevice();
	init_info.Device = vkContext.getVulkanDevice();
	init_info.DescriptorPool = vkContext.getDescriptorPool();
	init_info.Queue = vkContext.getGraphicsQueue();
	init_info.PipelineCache = nullptr;
	init_info.Allocator = nullptr;
	init_info.MinImageCount = swapchainInfo.minImageCount;
	init_info.ImageCount = static_cast<uint32_t>(vkContext.numImages());

	initializeFonts = [&vkContext, this]() {
		VkCommandBuffer commandBuffer = vkContext.allocTransientBuffer();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		vkContext.submitTransientBuffer(commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	};

	window = vkContext.getWindow();
	fbWidth = context.width();
	fbHeight = context.height();
	dirty = true;
}

void VulkanImGUI::apply(IRenderPass& renderPass) {
	if (dirty) {
		initImGUI(renderPass);
		dirty = false;
	}

	ImGui_ImplVulkan_NewFrame();

    if( window ) {
        std::atomic<bool> done{false};
        std::function<void()> f = [&done](){
            ImGui_ImplGlfw_NewFrame();
            done = true;
        };
        CreateApplication()->runOnUIThread( f );
        while( !done )
            std::this_thread::yield();
    }
}

void VulkanImGUI::update() {
}

void VulkanImGUI::render(IRenderPass& renderPass) {
	VulkanRenderPass& vkRenderPass{ dynamic_cast<VulkanRenderPass&>(renderPass) };

	ImDrawData* draw_data = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(draw_data, vkRenderPass.commandBuffer());
}

RENDERER_EXPORT std::shared_ptr<IImGUI> CreateImGUI() {
	return std::make_shared<VulkanImGUI>();
}
