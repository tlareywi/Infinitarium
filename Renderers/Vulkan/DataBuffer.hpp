//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include "../../Engine/DataBuffer.hpp"
#include "RenderContext.hpp"

#include "vulkan/vulkan.h"

class VulkanRenderContext;

class VulkanBuffer : public IDataBuffer {
public:
	VulkanBuffer(VulkanRenderContext& c);
	virtual ~VulkanBuffer();

	void commit() override;
	void set(DataPackContainer&) override;
	void reserve(unsigned int sizeBytes) override;
	void set(const void* const, unsigned int sizeBytes) override;
	void set(const void* const, unsigned int offset, unsigned int sizeBytes) override;
	void copy(IRenderTarget&, const glm::uvec4&) override;
	void getData(void*) override;
	void getData(const glm::uvec4&, size_t, size_t, void*) override;

	VkBuffer const getVkBuffer() const {
		return gpu;
	}

	VkBuffer const getStagingBuffer() const {
		return cpu;
	}

	VkDeviceSize length() const {
		return bufferSize;
	}

	// TODO: Eventuall should re-factor to a memory manager class. Along with some other things currently baked into VulkanBuffer.
	static uint32_t findMemoryType(const VulkanRenderContext&, uint32_t, VkMemoryPropertyFlags);

private:
	void createBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);
	void copyBuffer(VkBuffer, VkBuffer, VkDeviceSize);
	void copyBuffer(VkBuffer, VkBuffer, const glm::uvec4&, size_t, size_t);
	VkBufferUsageFlagBits translateUsage();

	VkDeviceSize bufferSize;

	// Dest buffer for CPU -> GPU transfers
	VkBuffer gpu;
	VkDeviceMemory gpuBufferMemory;

	// Source buffer For CPU -> GPU transfers
	VkBuffer cpu;
	VkDeviceMemory stagingBufferMemory;

	// Dest buffer for GPU -> CPU transfers
	VkBuffer xfer;
	VkDeviceMemory xferBufferMemory;
	size_t xferSz{0};

	void* map;

	VulkanRenderContext& context;
};