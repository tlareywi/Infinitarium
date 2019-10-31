#include "DataBuffer.hpp"

__declspec(dllexport) std::shared_ptr<IDataBuffer> CreateDataBuffer() {
	return std::make_shared<VulkanBuffer>();
}