#include "../../config.h"
#if USE_OPENXR

#include "OpenXRContext.hpp"
#include "xr_linear.h"

#include <algorithm>

OpenXRContext::OpenXRContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs, bool headset) : VulkanRenderContext(x, y, w, h, fs, headset) {

}

OpenXRContext::OpenXRContext(const IRenderContext& obj) : VulkanRenderContext(obj) {

}

OpenXRContext::~OpenXRContext() {

}

void OpenXRContext::attachTargets(IRenderPass& renderPass) {
	unsigned int i{ 0 };
	for (auto& swapChainPair : xrSwapchainTargets) {
		for (auto& target : swapChainPair.second) {
			target.setExtent(swapchains[i].createInfo.width, swapchains[i].createInfo.height);
			target.attach(*this, renderPass);
		}
		++i;
	}
}

void OpenXRContext::setSurface(void* params) {
	ContextParams* cxParams = reinterpret_cast<ContextParams*>(params);
	
	// Check what blend mode is valid for this device (opaque vs transparent displays)
	// We'll just take the first one available!
	uint32_t blend_count = 0;
	xrEnumerateEnvironmentBlendModes(cxParams->xrInstance, cxParams->xrSystemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 1, &blend_count, &xrBlendMode);

	// OpenXR wants to ensure apps are using the correct graphics card, so this MUST be called 
	// before xrCreateSession. This is crucial on devices that have multiple graphics cards, 
	// like laptops with integrated graphics chips in addition to dedicated graphics cards.
	XrGraphicsRequirementsVulkan2KHR requirement = { XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN2_KHR, nullptr, 0, 0 };
	if( GetVulkanGraphicsRequirements2KHR(cxParams->xrInstance, cxParams->xrSystemId, &requirement) != XR_SUCCESS )
		throw std::runtime_error("Fatal: No appropriate interface found supporting Vulkan renderer with headset.");

	XrVulkanGraphicsDeviceGetInfoKHR deviceGetInfo{ XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR };
	deviceGetInfo.systemId = cxParams->xrSystemId;
	deviceGetInfo.vulkanInstance = cxParams->vkInstance;
	if (GetVulkanGraphicsDevice2KHR(cxParams->xrInstance, &deviceGetInfo, &physicalDevice) != XR_SUCCESS)
		throw std::runtime_error("Fatal: Failed to initialize Vulkan device under XR.");

	VkDeviceCreateInfo& deviceCreateInfo = createDeviceQueueInfo(physicalDevice, cxParams->surface);

	XrVulkanDeviceCreateInfoKHR xrDeviceCreateInfo{ XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR };
	xrDeviceCreateInfo.systemId = cxParams->xrSystemId;
	xrDeviceCreateInfo.pfnGetInstanceProcAddr = &vkGetInstanceProcAddr;
	xrDeviceCreateInfo.vulkanCreateInfo = &deviceCreateInfo;
	xrDeviceCreateInfo.vulkanPhysicalDevice = physicalDevice;
	xrDeviceCreateInfo.vulkanAllocator = nullptr;
	VkResult vkResult;
	if (CreateVulkanDeviceKHR(cxParams->xrInstance, &xrDeviceCreateInfo, &logicalDevice, &vkResult) != XR_SUCCESS || vkResult != VK_SUCCESS)
		throw std::runtime_error("Fatal: Failed to initialize Vulkan logical device under XR.");

	createDeviceQueue();

	// A session represents this application's desire to display things! This is where we hook up our graphics API.
	// This does not start the session, for that, you'll need a call to xrBeginSession, which we do in openxr_poll_events
	XrGraphicsBindingVulkan2KHR binding = { XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR, nullptr };
	binding.device = logicalDevice;
	binding.physicalDevice = physicalDevice;
	binding.instance = cxParams->vkInstance;
	binding.queueFamilyIndex = graphicsQueueIndx;
	binding.queueIndex = 0; // Hardcoded in parent class
	XrSessionCreateInfo sessionInfo = { XR_TYPE_SESSION_CREATE_INFO };
	sessionInfo.next = &binding;
	sessionInfo.systemId = cxParams->xrSystemId;
	if( xrCreateSession(cxParams->xrInstance, &sessionInfo, &xrSession) != XR_SUCCESS )
		throw std::runtime_error("Fatal: Unable to initialize XR session. Is headset attached and in ready state?");

	// OpenXR uses a couple different types of reference frames for positioning content, we need to choose one for
	// displaying our content! STAGE would be relative to the center of your guardian system's bounds, and LOCAL
	// would be relative to your device's starting location.
	XrReferenceSpaceCreateInfo refSpace = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
	refSpace.poseInReferenceSpace = { {0,0,0,1}, {0,0,0} };
	refSpace.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
	xrCreateReferenceSpace(xrSession, &refSpace, &xrAppSpace);

	// We expect 2 views since we've explicitly configured for stereo headset. 
	uint32_t view_count = 0;
	xrEnumerateViewConfigurationViews(cxParams->xrInstance, cxParams->xrSystemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 0, &view_count, nullptr);
	xrConfigViews.resize(view_count, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
	xrViews.resize(view_count, { XR_TYPE_VIEW });
	xrEnumerateViewConfigurationViews(cxParams->xrInstance, cxParams->xrSystemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, view_count, &view_count, xrConfigViews.data());

	swapchains.reserve(view_count);
	views.resize(view_count);
	uint32_t imageCount{ 0 };

	projectionLayerViews.resize(view_count);

	for (uint32_t i = 0; i < view_count; i++) {
		// Create a swapchain for each view.
		XrViewConfigurationView& view = xrConfigViews[i];
		Swapchain swapchain;
	    swapchain.createInfo.type = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
		swapchain.createInfo.arraySize = 1;
		swapchain.createInfo.mipCount = 1;
		swapchain.createInfo.faceCount = 1;
		swapchain.createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		swapchain.createInfo.width = view.recommendedImageRectWidth;
		swapchain.createInfo.height = view.recommendedImageRectHeight;
		swapchain.createInfo.sampleCount = view.recommendedSwapchainSampleCount;
		swapchain.createInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
		if (xrCreateSwapchain(xrSession, &swapchain.createInfo, &swapchain.handle) != XR_SUCCESS)
			throw std::runtime_error("Fatal: Failed to create XR swapchain for one or more views.");

		swapchains.push_back(swapchain);

		xrEnumerateSwapchainImages(swapchain.handle, 0, &imageCount, nullptr);
		std::vector<XrSwapchainImageVulkanKHR> swapchainImage(imageCount, {XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR, nullptr, nullptr});
		xrEnumerateSwapchainImages(swapchain.handle, imageCount, &imageCount, reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchainImage.data()));

		// Render targets (image + imageview + framebuffer)
		std::vector<VulkanRenderTarget> swapchainTargets;
		swapchainTargets.reserve(imageCount);
		for (size_t i = 0; i < imageCount; i++) {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapchainImage[i].image;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VulkanRenderTarget target(logicalDevice, createInfo);
			swapchainTargets.emplace_back(std::move(target));
		}

		xrSwapchainTargets.insert(std::make_pair(swapchain.handle, std::move(swapchainTargets)));

		// CLUDGE: RenderPass currently uses the base classe's swapchain info for initalizing the framebuffer resouces. The views may not be the same 
		// size but typically will be for our purposes. 
		swapchainCreateInfo.imageExtent.width = view.recommendedImageRectWidth;
		swapchainCreateInfo.imageExtent.height = view.recommendedImageRectHeight;
		swapchainCreateInfo.imageFormat = (VkFormat)swapchain.createInfo.format;
		swapchainCreateInfo.minImageCount = imageCount;

		setContextExtent(view.recommendedImageRectWidth, view.recommendedImageRectHeight);
	}

	createDescriptorPool();
	xrInstance = cxParams->xrInstance;
	vkInstance = cxParams->vkInstance;
}

VulkanRenderTarget& OpenXRContext::getSwapchainTarget() {
	return xrSwapchainTargets[swapchains[activeSwapchain].handle][swapchainImageIndex];
}

void OpenXRContext::submit(VkCommandBuffer buffer, VkFence, VkSemaphore) {
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buffer;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, nullptr) != VK_SUCCESS)
		throw std::runtime_error("failed to submit draw command buffer!");

	XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
	xrReleaseSwapchainImage(swapchains[activeSwapchain].handle, &releaseInfo);
}

void OpenXRContext::beginFrame() {
	// TODO: Refactor
	bool exitRenderLoop{ false }, requestRestart{false};
	PollEvents(&exitRenderLoop, &requestRestart);
	///

	XrFrameWaitInfo frameWaitInfo{ XR_TYPE_FRAME_WAIT_INFO };
	if(xrWaitFrame(xrSession, &frameWaitInfo, &frameState) != XR_SUCCESS)
		throw std::runtime_error("Fatal: xrWaitFrame failed.");

	XrFrameBeginInfo frameBeginInfo{ XR_TYPE_FRAME_BEGIN_INFO };
	if( xrBeginFrame(xrSession, &frameBeginInfo) != XR_SUCCESS )
		throw std::runtime_error("Fatal: xrBeginFrame failed.");

	XrViewState viewState{ XR_TYPE_VIEW_STATE };
	XrViewLocateInfo viewLocateInfo{ XR_TYPE_VIEW_LOCATE_INFO };
	viewLocateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	viewLocateInfo.displayTime = frameState.predictedDisplayTime;
	viewLocateInfo.space = xrAppSpace;

	XrResult res = xrLocateViews(xrSession, &viewLocateInfo, &viewState, (uint32_t)views.size(), &viewCountOut, views.data());
	assert(res == XR_SUCCESS || res == XR_SESSION_LOSS_PENDING);
}

void OpenXRContext::endFrame() {
	layer.space = xrAppSpace;
	layer.viewCount = (uint32_t)projectionLayerViews.size();
	layer.views = projectionLayerViews.data();

	std::vector<XrCompositionLayerBaseHeader*> layers;
	layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer));

	XrFrameEndInfo frameEndInfo{ XR_TYPE_FRAME_END_INFO };
	frameEndInfo.displayTime = frameState.predictedDisplayTime;
	frameEndInfo.environmentBlendMode = xrBlendMode;
	frameEndInfo.layerCount = layers.size();
	frameEndInfo.layers = layers.data();
	if( xrEndFrame(xrSession, &frameEndInfo) != XR_SUCCESS )
		throw std::runtime_error("Fatal: xrEndFrame failed.");
}

unsigned int OpenXRContext::getPerspectiveCount() {
	return viewCountOut;
}

void OpenXRContext::getPerspective( unsigned int indx, glm::dmat4& proj, glm::dmat4& view ) {
	activeSwapchain = indx;

	XrMatrix4x4f projection_matrix;
	XrMatrix4x4f_CreateProjectionFov(&projection_matrix, GRAPHICS_VULKAN, views[indx].fov, 0.001f, 1000.0f);
	// TODO: tweak projection matrix for infinite z

	XrMatrix4x4f view_matrix;

	XrVector3f identity = { 1.0, 1.0, 1.0 };
	XrVector3f pos = { 0.0, 0.0, 0.0 };
	XrQuaternionf orientation = views[indx].pose.orientation;
	orientation.y = -orientation.y;
	XrMatrix4x4f_CreateTranslationRotationScale(&view_matrix, &pos, &orientation, &identity);
	//XrMatrix4x4f_CreateTranslationRotationScale(&view_matrix, &views[indx].pose.position, &views[indx].pose.orientation, &identity);

	proj = glm::mat4x4( 
		projection_matrix.m[0], projection_matrix.m[1], projection_matrix.m[2], projection_matrix.m[3],
		projection_matrix.m[4], projection_matrix.m[5], projection_matrix.m[6], projection_matrix.m[7],
		projection_matrix.m[8], projection_matrix.m[9], projection_matrix.m[10], projection_matrix.m[11],
		projection_matrix.m[12], projection_matrix.m[13], projection_matrix.m[14], projection_matrix.m[15]
	);

	view = glm::mat4x4(
		view_matrix.m[0], view_matrix.m[1], view_matrix.m[2], view_matrix.m[3],
		view_matrix.m[4], view_matrix.m[5], view_matrix.m[6], view_matrix.m[7],
		view_matrix.m[8], view_matrix.m[9], view_matrix.m[10], view_matrix.m[11],
		view_matrix.m[12], view_matrix.m[13], view_matrix.m[14], view_matrix.m[15]
	);

	// Get next image from active swapchain 
	const Swapchain& viewSwapchain = swapchains[activeSwapchain];
	XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
	xrAcquireSwapchainImage(viewSwapchain.handle, &acquireInfo, &swapchainImageIndex);

	XrSwapchainImageWaitInfo waitInfo{ XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
	waitInfo.timeout = XR_INFINITE_DURATION;
	xrWaitSwapchainImage(viewSwapchain.handle, &waitInfo);

	projectionLayerViews[activeSwapchain] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
	projectionLayerViews[activeSwapchain].pose = views[activeSwapchain].pose;
	projectionLayerViews[activeSwapchain].fov = views[activeSwapchain].fov;
	projectionLayerViews[activeSwapchain].subImage.swapchain = viewSwapchain.handle;
	projectionLayerViews[activeSwapchain].subImage.imageRect.offset = { 0, 0 };
	projectionLayerViews[activeSwapchain].subImage.imageRect.extent = { (int32_t)viewSwapchain.createInfo.width, (int32_t)viewSwapchain.createInfo.height };
}

const XrEventDataBaseHeader* OpenXRContext::TryReadNextEvent() {
	// It is sufficient to clear the just the XrEventDataBuffer header to
	// XR_TYPE_EVENT_DATA_BUFFER
	XrEventDataBaseHeader* baseHeader = reinterpret_cast<XrEventDataBaseHeader*>(&eventDataBuffer);
	*baseHeader = { XR_TYPE_EVENT_DATA_BUFFER };
	const XrResult xr = xrPollEvent(xrInstance, &eventDataBuffer);
	if (xr == XR_SUCCESS) {
		if (baseHeader->type == XR_TYPE_EVENT_DATA_EVENTS_LOST) {
			const XrEventDataEventsLost* const eventsLost = reinterpret_cast<const XrEventDataEventsLost*>(baseHeader);
			std::cout << eventsLost << " events lost." << std::endl;
		}

		return baseHeader;
	}
	if (xr == XR_EVENT_UNAVAILABLE) {
		return nullptr;
	}

	throw std::runtime_error("Unexpected error code from xrPollEvent.");
}

void OpenXRContext::PollEvents(bool* exitRenderLoop, bool* requestRestart) {
	*exitRenderLoop = *requestRestart = false;

	// Process all pending messages.
	while (const XrEventDataBaseHeader* event = TryReadNextEvent()) {
		switch (event->type) {
		case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
			const auto& instanceLossPending = *reinterpret_cast<const XrEventDataInstanceLossPending*>(event);
			std::cout << "XrEventDataInstanceLossPending by " << instanceLossPending.lossTime << std::endl;
			*exitRenderLoop = true;
			*requestRestart = true;
			return;
		}
		case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
			auto sessionStateChangedEvent = *reinterpret_cast<const XrEventDataSessionStateChanged*>(event);
			HandleSessionStateChangedEvent(sessionStateChangedEvent, exitRenderLoop, requestRestart);
			break;
		}
		case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
		//	LogActionSourceName(m_input.grabAction, "Grab");
		//	LogActionSourceName(m_input.quitAction, "Quit");
		//	LogActionSourceName(m_input.poseAction, "Pose");
		//	LogActionSourceName(m_input.vibrateAction, "Vibrate");
			break;
		case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
		default: {
			std::cout << "Ignoring event type " << event->type << std::endl;
			break;
		}
		}
	}
}

void OpenXRContext::HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent, bool* exitRenderLoop, bool* requestRestart) {
	switch (stateChangedEvent.state) {
	case XR_SESSION_STATE_READY: {
		XrSessionBeginInfo sessionBeginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
		sessionBeginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
		if (xrBeginSession(xrSession, &sessionBeginInfo) != XR_SUCCESS)
			throw std::runtime_error("Fatal: xrBeginSession failed.");

		break;
	}
	case XR_SESSION_STATE_STOPPING: {
		xrEndSession(xrSession);
		break;
	}
	case XR_SESSION_STATE_EXITING: {
		*exitRenderLoop = true;
		// Do not attempt to restart because user closed this session.
		*requestRestart = false;
		break;
	}
	case XR_SESSION_STATE_LOSS_PENDING: {
		*exitRenderLoop = true;
		// Poll for a new instance.
		*requestRestart = true;
		break;
	}
	default:
		break;
	}
}

#endif