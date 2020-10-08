#include "RenderPass.hpp"
#include "RenderContext.hpp"

VulkanRenderPass::~VulkanRenderPass() {
	if (device) {
		vkDestroyRenderPass(device, renderPass, nullptr);
		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}
	}
}

void VulkanRenderPass::prepare(IRenderContext& context) {
	VulkanRenderContext* vkContext = dynamic_cast<VulkanRenderContext*>(&context);
	device = vkContext->getVulkanDevice();
	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	vkContext->getVulkanSwapchainInfo(swapchainCreateInfo);

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapchainCreateInfo.imageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : Images to be presented in the swap chain
	// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL : Images to be used as destination for a memory copy operation
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create render pass!");
	}

	// Swapchain framebuffers.
	const std::vector<VkImageView>& swapChainImageViews{ vkContext->getImageViews() };
	swapChainFramebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapchainCreateInfo.imageExtent.width;
		framebufferInfo.height = swapchainCreateInfo.imageExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

	// Command buffers
	{
		commandBuffers.resize(swapChainFramebuffers.size());
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = vkContext->getCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}
}

void VulkanRenderPass::begin(std::shared_ptr<IRenderContext>& context) {
	activeContext = dynamic_cast<VulkanRenderContext*>(context.get());

	for (auto& target : targets) {
		if (target->getResource() == IRenderTarget::FrameBuffer) {
			swapChainIndx = activeContext->nextSwapChainTarget();
		}
		else {
			assert( false ); // Implement off screen render targets
		}
	}

	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	activeContext->getVulkanSwapchainInfo(swapchainCreateInfo);

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[swapChainIndx];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchainCreateInfo.imageExtent;
	VkClearValue clearColor = { 0.0f, 0.0f, 1.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if( vkBeginCommandBuffer(commandBuffers[swapChainIndx], &beginInfo) != VK_SUCCESS ) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	vkCmdBeginRenderPass(commandBuffers[swapChainIndx], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanRenderPass::end() {
	vkCmdEndRenderPass(commandBuffers[swapChainIndx]);
	if( vkEndCommandBuffer(commandBuffers[swapChainIndx]) != VK_SUCCESS ) {
		throw std::runtime_error("failed to record command buffer!");
	}
	 
	activeContext->submit( commandBuffers[swapChainIndx], swapChainIndx );

	activeContext->present( swapChainIndx );

	swapChainIndx = 0;
}

__declspec(dllexport) std::shared_ptr<IRenderPass> CreateRenderPass() {
	return std::make_shared<VulkanRenderPass>();
}

__declspec(dllexport) std::shared_ptr<IRenderPass> CreateRenderPassCopy( const IRenderPass& rp ) {
	return std::make_shared<VulkanRenderPass>( rp );
}


