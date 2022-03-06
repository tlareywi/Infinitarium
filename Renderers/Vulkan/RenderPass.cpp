#define NOMINMAX // TODO: move to cmakelists

#pragma warning(push)
#pragma warning( disable : 26812 ) // Unscoped enum warnings from Vulkan headers

#include "RenderPass.hpp"
#include "RenderContext.hpp"
#include "DataBuffer.hpp"

#include <algorithm>
#include <array>

#pragma warning(pop)

VulkanRenderPass::~VulkanRenderPass() {
	//renderTargetStacks.clear();

	if (device) {
		vkDestroyRenderPass(device, renderPass, nullptr);
	}
}

void VulkanRenderPass::prepare(IRenderContext& context) {
	VulkanRenderContext& vkContext = dynamic_cast<VulkanRenderContext&>(context);
	device = vkContext.getVulkanDevice();

	if (renderPass) {
		vkDestroyRenderPass(device, renderPass, nullptr);
	}

	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	vkContext.getVulkanSwapchainInfo(swapchainCreateInfo);

	std::vector<VkAttachmentDescription> attachments;
	std::vector<VkAttachmentReference> inputAttachmentRefs;
	std::vector<VkAttachmentReference> attachmentRefs;
	attachments.reserve(targets.size());
	attachmentRefs.reserve(targets.size());
	inputAttachmentRefs.reserve(attachments.size());

	std::vector<VkSubpassDependency> dependency;
	dependency.reserve(2);
	VkSubpassDependency dep = {};
	dep.srcSubpass = VK_SUBPASS_EXTERNAL;
	dep.dstSubpass = 0;
	dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep.srcAccessMask = 0;
	dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.emplace_back(std::move(dep));

	unsigned int indx{ 0 };

	// TODO: Currently, only one output target is really supported. Since we accomplish things like the pick buffer using a literally a buffer
	// instead of MRT in this architecture, maybe we need just one target here and no loop? Leaving for now until more things shake out. 
	for (auto& target : targets) { // Output RenderTargets 
		VulkanRenderTarget* vkTarget = dynamic_cast<VulkanRenderTarget*>(target.get());

		VkAttachmentDescription colorAttachment = {};
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if( loadOps[indx] == LoadOp::CLEAR )
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		else if( loadOps[indx] == LoadOp::LOAD )
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		else
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		if (target->getResource() == IRenderTarget::Resource::Swapchain) {
			target->setExtent(swapchainCreateInfo.imageExtent.width, swapchainCreateInfo.imageExtent.height);
			colorAttachment.format = swapchainCreateInfo.imageFormat;
			colorAttachment.finalLayout = vkContext.swapchainLayout();
			colorAttachmentRef.layout = vkContext.attachmentLayout();
			targetsSwapchain = true;
		}
		else if (target->getResource() == IRenderTarget::Resource::Offscreen) {
			vkTarget->initOffScreen( vkContext, *vkTarget );
			colorAttachment.format = vkTarget->getPixelFormat();
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			// See comments about renderstacks for offscreen targets below.
			framebufferAttachments.insert(framebufferAttachments.end(), vkTarget->getFramebufferAttachments().begin(), vkTarget->getFramebufferAttachments().end());

			// Auto layout transition to use offscreen buffer as shader input in future pass
			VkSubpassDependency transitionDep = {};
			transitionDep.srcSubpass = 0;
			transitionDep.dstSubpass = VK_SUBPASS_EXTERNAL;
			transitionDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			transitionDep.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			transitionDep.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			transitionDep.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependency.emplace_back(std::move(transitionDep));
		}

		attachments.emplace_back(std::move(colorAttachment));

		colorAttachmentRef.attachment = static_cast<uint32_t>(attachments.size()) - 1;
		attachmentRefs.emplace_back(std::move(colorAttachmentRef));

		++indx;
	}

	// FUTURE USE; these are for use in sub-passes and have the limitation of only being able to sample the same
	// fragment as the target in the pass. However, it takes advantage of tiled rendering which is fast (avoids
	// store/load ops). https://www.saschawillems.de/blog/2018/07/19/vulkan-input-attachments-and-sub-passes/
	for (auto& attachment : this->attachments) { // Input attachments
		VulkanRenderTarget* vkTarget = dynamic_cast<VulkanRenderTarget*>(attachment.get());

		VkAttachmentDescription colorAttachment = {};
		VkAttachmentReference colorAttachmentRef = {};

		colorAttachment.format = vkTarget->getPixelFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		attachments.emplace_back(std::move(colorAttachment));

		colorAttachmentRef.attachment = static_cast<uint32_t>(attachments.size()) - 1;
		inputAttachmentRefs.emplace_back(std::move(colorAttachmentRef));
	}

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = static_cast<uint32_t>(attachmentRefs.size());
	subpass.pColorAttachments = attachmentRefs.data();
	subpass.inputAttachmentCount = static_cast<uint32_t>(inputAttachmentRefs.size());
	subpass.pInputAttachments = inputAttachmentRefs.data();

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependency.size());
	renderPassInfo.pDependencies = dependency.data();

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create render pass!");
	}
	
	for (auto& target : targets) { // Attach must be called after the vkRenderPass is initialized
		if (target->getResource() != IRenderTarget::Resource::Swapchain) {
			// Here we create a stack of render target clones, one for each swapchain image for parallelism. Update; no we don't :P
			// The issue is then you need to somehow rotate the descriptors for anything that consumes the offscreen target. This gets
			// crazy fast and changing descriptors per frame can be slow too.  
			auto derivedPtr{ std::dynamic_pointer_cast<VulkanRenderTarget>(target) };
			//renderTargetStacks.emplace(derivedPtr, std::make_unique<RenderTargetStack>(*vkContext, *target, static_cast<uint8_t>(vkContext->numImages())));
			derivedPtr->attach(vkContext, *this);
		}
		else
			vkContext.attachSwapchain(*this); // Context maintains the swapchain RenderTargetStack
	}

	//for (auto& stack : renderTargetStacks) {
	//	stack.second->attach(context, *this);
	//}
}

VkCommandBuffer VulkanRenderPass::commandBuffer() {
	if (currentTarget)
		return currentTarget->getCmdBuffer();
	else
		return nullptr;
}

void VulkanRenderPass::begin(IRenderContext& context) {
	VulkanRenderContext& vkContext = dynamic_cast<VulkanRenderContext&>(context);

	if (targetsSwapchain) // At least one target is swapchain
		currentTarget = &vkContext.getSwapchainTarget();
	else { // All targets offscreen. 
		auto ref = *(targets.begin());
		currentTarget = dynamic_cast<VulkanRenderTarget*>(ref.get());
		//currentTarget = &(*renderTargetStacks.begin()->second)[vkContext.getTargetInFlight()];
	}

	auto fbResource = currentTarget->getFramebuffer(*this);
	if (!fbResource || !currentTarget->getCmdBuffer()) 
		return;

	VkFramebuffer fb = (*fbResource)();

	glm::vec4 cc = currentTarget->getClearColor();
	const VkClearValue clearColor{ cc.r, cc.g, cc.b, cc.a };

	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	vkContext.getVulkanSwapchainInfo(swapchainCreateInfo);

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = fb;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = { fbResource->getInfo().width, fbResource->getInfo().height };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	VkFence vkFence{ currentTarget->getFence() };
	VkResult result = vkWaitForFences(vkContext.getVulkanDevice(), 1, &vkFence, VK_TRUE, UINT64_MAX);
	assert(result == VK_SUCCESS);
	
	// TODO: Maybe makes more sense to have a stack of command buffers, one per swapchain image, tied to a RenderPass
	// as opposed to a command buffer on every RenderTarget? Maybe 6 to one, half dozen to the other. Just seems odd
	// in the MRT case to pick one command buffer arbitarily from multiple targets. 
	if( vkBeginCommandBuffer(currentTarget->getCmdBuffer(), &beginInfo) != VK_SUCCESS )
		throw std::runtime_error("failed to begin recording command buffer!");

	if (vkContext.isNewFrame()) { // Need to zero engine buffers on the first pass on a new frame.
		{
			auto buf = context.pickBuffer();
			VulkanBuffer& vkBuffer = dynamic_cast<VulkanBuffer&>(*buf);
			vkCmdFillBuffer(currentTarget->getCmdBuffer(), vkBuffer.getVkBuffer(), 0, (VkDeviceSize)context.width() * (VkDeviceSize)context.height() * sizeof(PickUnit), 0); 
		}
		{
			auto buf = context.postProcBuffer();
			VulkanBuffer& vkBuffer = dynamic_cast<VulkanBuffer&>(*buf);
			vkCmdFillBuffer(currentTarget->getCmdBuffer(), vkBuffer.getVkBuffer(), 0, (VkDeviceSize)context.width() * (VkDeviceSize)context.height() * sizeof(PostProcessUnit), 0); 
		}
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
	
	std::vector<VkSemaphore> waitSemaphores;
	waitSemaphores.reserve(attachments.size());
	for (auto& attachment : attachments) {
		auto derivedPtr{ std::dynamic_pointer_cast<VulkanRenderTarget>(attachment) };
		//auto& stack = *renderTargetStacks[derivedPtr];
		//waitSemaphores.emplace_back(stack[vkContext.getTargetInFlight()].getSemaphore());
		waitSemaphores.emplace_back(derivedPtr->getSemaphore());
	}

	std::vector<VkSemaphore> signalSemaphores;
	signalSemaphores.reserve(targets.size());
	for (auto& target : targets) {
		if (target->getResource() == IRenderTarget::Resource::Swapchain) {
			signalSemaphores.emplace_back(currentTarget->getSemaphore());		
		}
		else {
			auto derivedPtr{ std::dynamic_pointer_cast<VulkanRenderTarget>(target) };
			//auto& stack = *renderTargetStacks[derivedPtr];
			//signalSemaphores.emplace_back(stack[vkContext.getTargetInFlight()].getSemaphore());
			signalSemaphores.emplace_back(derivedPtr->getSemaphore());
		}
	}

	vkContext.submit( currentTarget->getCmdBuffer(), currentTarget->getFence(), waitSemaphores, signalSemaphores );
}

RENDERER_EXPORT std::shared_ptr<IRenderPass> CreateRenderPass() {
	return std::make_shared<VulkanRenderPass>();
}

RENDERER_EXPORT std::shared_ptr<IRenderPass> CreateRenderPassCopy( const IRenderPass& rp ) {
	return std::make_shared<VulkanRenderPass>( rp );
}


