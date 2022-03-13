//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "DataBuffer.hpp"

VulkanBuffer::VulkanBuffer(VulkanRenderContext& c) : context(c), cpu(nullptr), gpu(nullptr), xfer(nullptr), map(nullptr), 
stagingBufferMemory(nullptr), gpuBufferMemory(nullptr), xferBufferMemory(nullptr), bufferSize(0) {

}

VulkanBuffer::~VulkanBuffer() {
    if (cpu) {
        vkDestroyBuffer(context.getVulkanDevice(), cpu, nullptr);
        vkFreeMemory(context.getVulkanDevice(), stagingBufferMemory, nullptr);
    }
    if (gpu) {
        vkDestroyBuffer(context.getVulkanDevice(), gpu, nullptr);
        vkFreeMemory(context.getVulkanDevice(), gpuBufferMemory, nullptr);
    }
    if (xfer) {
        vkDestroyBuffer(context.getVulkanDevice(), xfer, nullptr);
        vkFreeMemory(context.getVulkanDevice(), xferBufferMemory, nullptr);
    }
}

void VulkanBuffer::commit() {
    if (!cpu) return;

    if (map) {
        vkUnmapMemory(context.getVulkanDevice(), stagingBufferMemory);
        map = nullptr;
    }

    copyBuffer(cpu, gpu, bufferSize);

    if (getUsage() != IDataBuffer::Usage::Uniform) { 
        // We assume vertex buffers are not modified 'on the fly'. Once committed, we can free the cpu side memory buffer. 
        // But UBO's remain allocated on both sides of the fence. 
        vkDestroyBuffer(context.getVulkanDevice(), cpu, nullptr);
        cpu = nullptr;
        vkFreeMemory(context.getVulkanDevice(), stagingBufferMemory, nullptr);
        stagingBufferMemory = nullptr;
    }
}

void VulkanBuffer::set(DataPackContainer& container) {
    std::visit([this](auto& e) {
        bufferSize = e.sizeBytes();

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, cpu, stagingBufferMemory);

        void* data{ nullptr };
        vkMapMemory(context.getVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, e.get(), (size_t)bufferSize);
        vkUnmapMemory(context.getVulkanDevice(), stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | translateUsage(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, gpu, gpuBufferMemory);

        setStride(e.getStride());

    }, container);
}

void VulkanBuffer::reserve(unsigned int sizeBytes) {
    if (bufferSize < sizeBytes) {
    	if( cpu ) {
	        vkDestroyBuffer(context.getVulkanDevice(), cpu, nullptr);
	        vkFreeMemory(context.getVulkanDevice(), stagingBufferMemory, nullptr);
    	}
        createBuffer(sizeBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, cpu, stagingBufferMemory);
    }

    if (bufferSize < sizeBytes) {
    	if( gpu ) {
	        vkDestroyBuffer(context.getVulkanDevice(), gpu, nullptr);
	        vkFreeMemory(context.getVulkanDevice(), gpuBufferMemory, nullptr);
    	}
        createBuffer(sizeBytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT | translateUsage(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, gpu, gpuBufferMemory);
    }

    bufferSize = sizeBytes;
}

void VulkanBuffer::set(const void* const data, unsigned int sizeBytes) {
   reserve(sizeBytes);

   vkMapMemory(context.getVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &map);
   memcpy(map, data, (size_t)bufferSize);
   vkUnmapMemory(context.getVulkanDevice(), stagingBufferMemory);
   map = nullptr;
}

void VulkanBuffer::set(const void* const data, unsigned int offset, unsigned int sizeBytes) {
    assert(sizeBytes <= bufferSize);

    // May be called many times in tight loop. Don't reallocate or unmap here.
    if (!map)
        vkMapMemory(context.getVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &map);

    memcpy((uint8_t*)(map) + offset, data, sizeBytes);
}

void VulkanBuffer::copy(IRenderTarget&, const glm::uvec4&) {
    // TODO: not yet implemented
}

void VulkanBuffer::getData(void* buf) {
    vkMapMemory(context.getVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &map);
    memcpy(buf, map, bufferSize);
    vkUnmapMemory(context.getVulkanDevice(), stagingBufferMemory);
    map = nullptr;
}

void VulkanBuffer::getData(const glm::uvec4& rect, size_t srcWidth, size_t bytesPerUnit, void* out) {
    size_t bytesRect{ (size_t)rect.z * (size_t)rect.w * bytesPerUnit };
    if (!xfer || xferSz < bytesRect) {
        vkDestroyBuffer(context.getVulkanDevice(), xfer, nullptr);
        vkFreeMemory(context.getVulkanDevice(), xferBufferMemory, nullptr);
        createBuffer(bytesRect, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, xfer, xferBufferMemory);
    }

    copyBuffer(gpu, xfer, rect, srcWidth, bytesPerUnit);

    void* bufferMap{ nullptr };
    vkMapMemory(context.getVulkanDevice(), xferBufferMemory, 0, bytesRect, 0, &bufferMap);
    memcpy(out, bufferMap, bytesRect);
    vkUnmapMemory(context.getVulkanDevice(), xferBufferMemory);
}

void VulkanBuffer::createBuffer( VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memProps, VkBuffer& buffer, VkDeviceMemory& bufferMemory ) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(context.getVulkanDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(context.getVulkanDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(context, memRequirements.memoryTypeBits, memProps);

    // TODO: It should be noted that in a real world application, you're not supposed to actually call vkAllocateMemory for every individual buffer. 
    // The maximum number of simultaneous memory allocations is limited by the maxMemoryAllocationCount physical device limit, which may be as low as 4096 
    // even on high end hardware like an NVIDIA GTX 1080. The right way to allocate memory for a large number of objects at the same time is to create a 
    // custom allocator that splits up a single allocation among many different objects by using the offset parameters that we've seen in many functions.

    // You can either implement such an allocator yourself, or use the VulkanMemoryAllocator library provided by the GPUOpen initiative.However, for this 
    // tutorial it's okay to use a separate allocation for every resource, because we won't come close to hitting any of these limits for now. 

    if (vkAllocateMemory(context.getVulkanDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(context.getVulkanDevice(), buffer, bufferMemory, 0);
}

uint32_t VulkanBuffer::findMemoryType( const VulkanRenderContext& context, uint32_t typeFilter, VkMemoryPropertyFlags properties ) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(context.getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        if( typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties )
            return i;

    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, const glm::uvec4& rect, size_t srcWidth, size_t bytesPerUnit) {
    VkCommandBuffer commandBuffer = context.allocTransientBuffer();

    for (size_t i = 0; i < rect.w; ++i ) {
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = (((size_t)rect.y + i) * srcWidth * bytesPerUnit) + rect.x * bytesPerUnit;
        copyRegion.dstOffset = i * (size_t)rect.z * bytesPerUnit;
        copyRegion.size = rect.z * bytesPerUnit;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    }

    context.submitTransientBuffer(commandBuffer);
}

void VulkanBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = context.allocTransientBuffer();

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; 
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    context.submitTransientBuffer(commandBuffer);
}

VkBufferUsageFlagBits VulkanBuffer::translateUsage() {
    switch (getUsage()) {
    case IDataBuffer::Usage::Uniform:
        return  static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    case IDataBuffer::Usage::Storage:
        return static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    case IDataBuffer::Usage::Pick: // Pick buffer requires readback from GPU so set transfer src bit.
        return static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    case IDataBuffer::Usage::PostProcess:
        return static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    case IDataBuffer::Usage::VertexAttribute:
    default:
        return static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    }
}


RENDERER_EXPORT std::shared_ptr<IDataBuffer> CreateDataBuffer( IRenderContext& c ) {
	return std::make_shared<VulkanBuffer>( *dynamic_cast<VulkanRenderContext*>(&c) );
}