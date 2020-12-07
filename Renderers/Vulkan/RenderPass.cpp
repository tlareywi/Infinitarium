#include "RenderPass.hpp"
#include "RenderContext.hpp"

VulkanRenderPass::~VulkanRenderPass() {
	if (device) {
		vkDestroyRenderPass(device, renderPass, nullptr);
	}
}

void VulkanRenderPass::prepare(IRenderContext& context) {
	VulkanRenderContext* vkContext = dynamic_cast<VulkanRenderContext*>(&context);
	device = vkContext->getVulkanDevice();

	if(renderPass)
		vkDestroyRenderPass(device, renderPass, nullptr);

	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	vkContext->getVulkanSwapchainInfo(swapchainCreateInfo);

	std::vector<VkAttachmentDescription> attachments;
	std::vector<VkAttachmentReference> attachmentRefs;
	attachments.reserve(targets.size());
	attachmentRefs.reserve(targets.size());

	bool attachSwapchain{ false };

	VkAttachmentReference colorAttachmentRef = {};

	for (auto& target : targets) {
		VulkanRenderTarget* vkTarget = dynamic_cast<VulkanRenderTarget*>(target.get());

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if( target->getClear() )
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		else
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		if (target->getResource() == IRenderTarget::Resource::Swapchain) {
			target->setExtent(swapchainCreateInfo.imageExtent.width, swapchainCreateInfo.imageExtent.height);
			colorAttachment.format = swapchainCreateInfo.imageFormat;
			colorAttachment.finalLayout = vkContext->swapchainLayout();
			colorAttachmentRef.layout = vkContext->attachmentLayout();
			attachSwapchain = true;
		}
		else if (target->getResource() == IRenderTarget::Resource::Offscreen) {
			continue; // TODO
			//dynamic_cast<VulkanRenderTarget*>(target.get())->getPixelFormat();
			//colorAttachment.format = dynamic_cast<VulkanRenderTarget*>(target.get())->getPixelFormat();
			//colorAttachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			//colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		attachments.emplace_back(std::move(colorAttachment));

		colorAttachmentRef.attachment = attachmentRefs.size();
		attachmentRefs.emplace_back(std::move(colorAttachmentRef));
	}

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = attachmentRefs.size();
	subpass.pColorAttachments = attachmentRefs.data();

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create render pass!");
	}
	
	for (auto& target : targets) { // Attach must be called after the vkRenderPass is initialized
		dynamic_cast<VulkanRenderTarget*>(target.get())->attach(context, *this); // TODO: Should this be moved up to the interface level?
	}

	if (attachSwapchain) { // This render pass declares a swapchain attachment. Associate this renderpass with the swapchain framebuffers.
		vkContext->attachTargets( *this );
	}
}

VkCommandBuffer VulkanRenderPass::commandBuffer() {
	if (currentTarget)
		return currentTarget->getCmdBuffer();
	else
		return nullptr;
}

void VulkanRenderPass::begin(IRenderContext& context) {
	VulkanRenderContext& vkContext = dynamic_cast<VulkanRenderContext&>(context);

	currentTarget = &vkContext.getSwapchainTarget();
	if (!currentTarget->getFramebuffer() || !currentTarget->getCmdBuffer()) return;

	glm::vec4 cc = currentTarget->getClearColor();
	const VkClearValue clearColor{ cc.r, cc.g, cc.b, cc.a };

	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	vkContext.getVulkanSwapchainInfo(swapchainCreateInfo);

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = currentTarget->getFramebuffer();
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchainCreateInfo.imageExtent;
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if( vkBeginCommandBuffer(currentTarget->getCmdBuffer(), &beginInfo) != VK_SUCCESS ) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	vkCmdBeginRenderPass(currentTarget->getCmdBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanRenderPass::end(IRenderContext& context) {
	if (!currentTarget->getCmdBuffer()) return;
	VulkanRenderContext& vkContext = dynamic_cast<VulkanRenderContext&>(context);

	vkCmdEndRenderPass(currentTarget->getCmdBuffer());
	if( vkEndCommandBuffer(currentTarget->getCmdBuffer()) != VK_SUCCESS ) {
		throw std::runtime_error("failed to record command buffer!");
	}
	 
	vkContext.submit( currentTarget->getCmdBuffer() );
}

__declspec(dllexport) std::shared_ptr<IRenderPass> CreateRenderPass() {
	return std::make_shared<VulkanRenderPass>();
}

__declspec(dllexport) std::shared_ptr<IRenderPass> CreateRenderPassCopy( const IRenderPass& rp ) {
	return std::make_shared<VulkanRenderPass>( rp );
}


