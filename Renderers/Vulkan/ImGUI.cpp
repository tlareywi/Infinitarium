#include "ImGUI.hpp"
#include "RenderContext.hpp"
#include "RenderPass.hpp"

VulkanImGUI::~VulkanImGUI() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}

void VulkanImGUI::initImGUI(IRenderPass& renderPass) {
	if (!init_info.CheckVkResultFn) { // First run
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.WantCaptureMouse = true;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForVulkan(window, true);
		init_info.CheckVkResultFn = [](VkResult result) {
			assert(result == VK_SUCCESS);
		};
	}
	else
		ImGui_ImplVulkan_Shutdown();

	VulkanRenderPass& vkRenderPass{ dynamic_cast<VulkanRenderPass&>(renderPass) };
	if( !ImGui_ImplVulkan_Init(&init_info, vkRenderPass.getVulkanRenderPass()) )
		throw("Failed to initialize ImGUI.");

	{ // Initialize Fonts 
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		assert( vkBeginCommandBuffer(commandBuffer, &begin_info) == VK_SUCCESS );

		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

		assert( vkEndCommandBuffer(commandBuffer) == VK_SUCCESS );

		vkSubmit();

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
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
	init_info.ImageCount = vkContext.numImages();

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vkContext.getCommandPool();
	allocInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(vkContext.getVulkanDevice(), &allocInfo, &commandBuffer);

	vkSubmit = [&vkContext, this]() {
		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &commandBuffer;

		vkContext.submit(end_info);
	};

	window = vkContext.getWindow();
}

void VulkanImGUI::apply(IRenderPass& renderPass) {
	if (dirty) {
		initImGUI(renderPass);
		dirty = false;
	}

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void VulkanImGUI::update() {
}

void VulkanImGUI::render(IRenderPass& renderPass) {
	VulkanRenderPass& vkRenderPass{ dynamic_cast<VulkanRenderPass&>(renderPass) };

	ImDrawData* draw_data = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(draw_data, vkRenderPass.commandBuffer());
}

__declspec(dllexport) std::shared_ptr<IImGUI> CreateImGUI() {
	return std::make_shared<VulkanImGUI>();
}
