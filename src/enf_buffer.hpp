#pragma once

#include "enf_device.hpp"

namespace Enforcer {

  class Buffer {
  public:
    Buffer(Device &device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags,
           VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment = 1);
    ~Buffer();

    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;

    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void unmap();

    void writeToBuffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    void writeToIndex(void *data, int index);
    VkResult flushIndex(int index);
    VkDescriptorBufferInfo descriptorInfoForIndex(int index);
    VkResult invalidateIndex(int index);

    VkBuffer getBuffer() const noexcept { return buffer; }
    void *getMappedMemory() const noexcept { return mapped; }
    uint32_t getInstanceCount() const noexcept { return instanceCount; }
    VkDeviceSize getInstanceSize() const noexcept { return instanceSize; }
    VkDeviceSize getAlignmentSize() const noexcept { return instanceSize; }
    VkBufferUsageFlags getUsageFlags() const noexcept { return usageFlags; }
    VkMemoryPropertyFlags getMemoryPropertyFlags() const noexcept { return memoryPropertyFlags; }
    VkDeviceSize getBufferSize() const { return bufferSize; }

  private:
    static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

    Device &lveDevice;
    void *mapped = nullptr;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    VkDeviceSize bufferSize;
    uint32_t instanceCount;
    VkDeviceSize instanceSize;
    VkDeviceSize alignmentSize;
    VkBufferUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryPropertyFlags;
  };

} // namespace Enforcer
