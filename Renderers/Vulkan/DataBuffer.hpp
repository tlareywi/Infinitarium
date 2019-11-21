#pragma once

#include "../../Engine/DataBuffer.hpp"

class VulkanBuffer : public IDataBuffer {
public:
	virtual ~VulkanBuffer() {}

	void commit() override {};
	void set(DataPackContainer&) override {};
	void reserve(unsigned int sizeBytes) override {};
	void set(const void* const, unsigned int sizeBytes) override {};
	void set(const void* const, unsigned int offset, unsigned int sizeBytes) override {};
	void copy(IRenderTarget&, const glm::uvec4&) override {};
	void getData(void*) override {};
};