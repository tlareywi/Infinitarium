#include "DataBuffer.hpp"

VulkanBuffer::VulkanBuffer(VulkanRenderContext& c) : context(c), cpu(nullptr), gpu(nullptr), map(nullptr), stride(0), 
stagingBufferMemory(nullptr), gpuBufferMemory(nullptr), bufferSize(0), format(VK_FORMAT_UNDEFINED) {

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
}

void VulkanBuffer::commit() {
    if (!cpu) return;

    if (map) {
        vkUnmapMemory(context.getVulkanDevice(), stagingBufferMemory);
        map = nullptr;
    }

    copyBuffer(cpu, gpu, bufferSize);

    if (getUsage() == IDataBuffer::Usage::VertexBuffer) { 
        // We assume vertex buffers are not modified 'on the fly'. Once committed, we can free the cpy side memory buffer. 
        // But UBO's remain allocated on both sides of the fence. 
        vkDestroyBuffer(context.getVulkanDevice(), cpu, nullptr);
        cpu = nullptr;
        vkFreeMemory(context.getVulkanDevice(), stagingBufferMemory, nullptr);
        stagingBufferMemory = nullptr;
    }
}

uint32_t VulkanBuffer::getStride() {
    return stride;
}

uint32_t VulkanBuffer::getFormat() {
    return static_cast<uint32_t>(format);
}

void VulkanBuffer::set(DataPackContainer& container) {
    std::visit([this](auto& e) {
        bufferSize = e.sizeBytes();

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, cpu, stagingBufferMemory);

        void* data{ nullptr };
        vkMapMemory(context.getVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, e.get(), (size_t)bufferSize);
        vkUnmapMemory(context.getVulkanDevice(), stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | translateUsage(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, gpu, gpuBufferMemory);

        commit();

        stride = e.getStride();
        parseFormat( e.getChannelsPerVertex(), e.getType() );

    }, container);
}

void VulkanBuffer::reserve(unsigned int sizeBytes) {
    if (!cpu || bufferSize < sizeBytes) {
        vkDestroyBuffer(context.getVulkanDevice(), cpu, nullptr);
        vkFreeMemory(context.getVulkanDevice(), stagingBufferMemory, nullptr);
        createBuffer(sizeBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, cpu, stagingBufferMemory);
    }

    if (!gpu || bufferSize < sizeBytes) {
        vkDestroyBuffer(context.getVulkanDevice(), gpu, nullptr);
        vkFreeMemory(context.getVulkanDevice(), gpuBufferMemory, nullptr);
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
    // TODO: Picking. Need render target and texture implementation first.
}

void VulkanBuffer::getData(void* buf) {
    vkMapMemory(context.getVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &map);
    memcpy(buf, map, bufferSize);
    vkUnmapMemory(context.getVulkanDevice(), stagingBufferMemory);
    map = nullptr;
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
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memProps);

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

uint32_t VulkanBuffer::findMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags properties ) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(context.getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        if( typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties )
            return i;

    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = context.getCommandPool();
    allocInfo.commandBufferCount = 1;

    // TODO:  You may wish to create a separate command pool for these kinds of short-lived buffers, because the 
    // implementation may be able to apply memory allocation optimizations. You should use the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
    // flag during command pool generation in that case.

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(context.getVulkanDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    context.submit( submitInfo );

    vkFreeCommandBuffers(context.getVulkanDevice(), context.getCommandPool(), 1, &commandBuffer);
}

void VulkanBuffer::parseFormat(uint8_t channelsPerVertex, DataType type) {
    if (type == DataType::FLOAT) {
        switch (channelsPerVertex) {
        case 1:
            format = VK_FORMAT_R32_SFLOAT;
            break;
        case 2:
            format = VK_FORMAT_R32G32_SFLOAT;
            break;
        case 3:
            format = VK_FORMAT_R32G32B32_SFLOAT;
            break;
        case 4:
            format = VK_FORMAT_R32G32B32A32_SFLOAT;
            break;
        default:
            break;
        }
    }
    else if (type == DataType::UINT32) {
        switch (channelsPerVertex) {
        case 1:
            format = VK_FORMAT_R32_UINT;
            break;
        case 2:
            format = VK_FORMAT_R32G32_UINT;
            break;
        case 3:
            format = VK_FORMAT_R32G32B32_UINT;
            break;
        case 4:
            format = VK_FORMAT_R32G32B32A32_UINT;
            break;
        default:
            break;
        }
    }
    else if (type == DataType::UINT16) {
        switch (channelsPerVertex) {
        case 1:
            format = VK_FORMAT_R16_UINT;
            break;
        case 2:
            format = VK_FORMAT_R16G16_UINT;
            break;
        case 3:
            format = VK_FORMAT_R16G16B16_UINT;
            break;
        case 4:
            format = VK_FORMAT_R16G16B16A16_UINT;
            break;
        default:
            break;
        }
    }
    else if (type == DataType::UINT8) {
        switch (channelsPerVertex) {
        case 1:
            format = VK_FORMAT_R8_UINT;
            break;
        case 2:
            format = VK_FORMAT_R8G8_UINT;
            break;
        case 3:
            format = VK_FORMAT_R8G8B8_UINT;
            break;
        case 4:
            format = VK_FORMAT_R8G8B8A8_UINT;
            break;
        default:
            break;
        }
    }
}

VkBufferUsageFlagBits VulkanBuffer::translateUsage() {
    switch (getUsage()) {
    case IDataBuffer::Usage::UniformBuffer:
        return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    case IDataBuffer::Usage::VertexBuffer:
    default:
        return static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT); // TODO: any benefit to separating thses out and making the difference explicit? 
    }
}

__declspec(dllexport) std::shared_ptr<IDataBuffer> CreateDataBuffer( IRenderContext& c ) {
	return std::make_shared<VulkanBuffer>( *dynamic_cast<VulkanRenderContext*>(&c) );
}