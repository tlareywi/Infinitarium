#pragma once

#include "../../Engine/DataBuffer.hpp"
#include "RenderContext.hpp"

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
	uint32_t getStride() override;
	uint32_t getFormat() override;

	VkBuffer getVkBuffer() {
		return gpu;
	}

	VkDeviceSize length() {
		return bufferSize;
	}

private:
	void createBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);
	uint32_t findMemoryType(uint32_t, VkMemoryPropertyFlags);
	void copyBuffer(VkBuffer, VkBuffer, VkDeviceSize);
	void copyBuffer(VkBuffer, VkBuffer, const glm::uvec4&, size_t, size_t);
	void parseFormat( uint8_t, DataType );
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

	uint32_t stride;
	VkFormat format;
};