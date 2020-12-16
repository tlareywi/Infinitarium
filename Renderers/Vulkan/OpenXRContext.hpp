#pragma once

#include "RenderContext.hpp"

#include <map>

class OpenXRContext : public VulkanRenderContext {
public:
    OpenXRContext(const IRenderContext& obj);
    OpenXRContext(unsigned int x, unsigned int y, unsigned int w, unsigned int h, bool fs, bool headset);
    virtual ~OpenXRContext();

    // IRenderContext /////////////////////////////////
	void* getSurface() override {
        return nullptr;
    };
	void setSurface(void*) override;
	void pauseRendering(bool) override {};
    void beginFrame() override;
    void endFrame() override;
    unsigned int getPerspectiveCount() override;
    void getPerspective(unsigned int, glm::mat4x4& proj, glm::mat4x4& view) override;
    void toggleFullScreen() override {};
   
    // VulkanRenderContext ////////////////////////////
    VkImageLayout swapchainLayout() override {
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    VkImageLayout attachmentLayout() override {
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    }
    VulkanRenderTarget& getSwapchainTarget() override;
    void attachTargets(IRenderPass& renderPass) override;
    size_t numImages() override {
        size_t n{ 0 };
        for( auto& p : xrSwapchainTargets )
            n += p.second.size();

        return n;
    }
    GLFWwindow* getWindow() override {
        return nullptr;
    }
    void submit(VkCommandBuffer, VkFence, VkSemaphore) override;

private:
    struct Swapchain {
        XrSwapchainCreateInfo createInfo;
        XrSwapchain handle;
    };
    
    XrInstance xrInstance{ nullptr };
    XrSession xrSession{nullptr};
    XrSpace xrAppSpace{nullptr};
    XrFrameState frameState{ XR_TYPE_FRAME_STATE };
    XrEnvironmentBlendMode xrBlendMode;
    std::vector<XrView> xrViews;
    std::vector<XrViewConfigurationView> xrConfigViews;
    std::vector<XrView> views;
    uint32_t viewCountOut;
    uint32_t activeSwapchain;
    uint32_t swapchainImageIndex;
    std::vector<Swapchain> swapchains;
    std::map<XrSwapchain, std::vector<VulkanRenderTarget>> xrSwapchainTargets;
    XrCompositionLayerProjection layer{ XR_TYPE_COMPOSITION_LAYER_PROJECTION };
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
    XrEventDataBuffer eventDataBuffer;

    void PollEvents(bool* exitRenderLoop, bool* requestRestart);
    const XrEventDataBaseHeader* TryReadNextEvent();
    void HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent, bool* exitRenderLoop, bool* requestRestart);

	XrResult GetVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsVulkan2KHR* graphicsRequirements) {
		PFN_xrGetVulkanGraphicsRequirements2KHR pfnGetVulkanGraphicsRequirements2KHR = nullptr;
		xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsRequirements2KHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetVulkanGraphicsRequirements2KHR));
		return pfnGetVulkanGraphicsRequirements2KHR(instance, systemId, graphicsRequirements);
	}

    XrResult CreateVulkanDeviceKHR(XrInstance instance, const XrVulkanDeviceCreateInfoKHR* createInfo, VkDevice* vulkanDevice, VkResult* vulkanResult) {
        PFN_xrCreateVulkanDeviceKHR pfnCreateVulkanDeviceKHR = nullptr;
        xrGetInstanceProcAddr(instance, "xrCreateVulkanDeviceKHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfnCreateVulkanDeviceKHR));
        return pfnCreateVulkanDeviceKHR(instance, createInfo, vulkanDevice, vulkanResult);
    }

    XrResult GetVulkanGraphicsDevice2KHR(XrInstance instance, const XrVulkanGraphicsDeviceGetInfoKHR* getInfo, VkPhysicalDevice* vulkanPhysicalDevice) {
        PFN_xrGetVulkanGraphicsDevice2KHR pfnGetVulkanGraphicsDevice2KHR = nullptr;
        xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsDevice2KHR", reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetVulkanGraphicsDevice2KHR));
        return pfnGetVulkanGraphicsDevice2KHR(instance, getInfo, vulkanPhysicalDevice);
    }
};