#include "enf_buffer.hpp"

// std
#include <cassert>
#include <cstring>

namespace Enforcer {

  /**
   * Returns the minimum instance size required to be compatible with devices
   * minOffsetAlignment
   *
   * @param instanceSize The size of an instance
   * @param minOffsetAlignment The minimum required alignment, in bytes, for the
   * offset member (eg minUniformBufferOffsetAlignment)
   *
   * @return VkResult of the buffer mapping call
   */
  VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
      return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
  }

  Buffer::Buffer(Device &device, const VkDeviceSize instanceSize, const uint32_t instanceCount, const VkBufferUsageFlags usageFlags,
                 const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize minOffsetAlignment) :
      lveDevice{device}, instanceCount{instanceCount}, instanceSize{instanceSize}, usageFlags{usageFlags},
      memoryPropertyFlags{memoryPropertyFlags} {
    alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    bufferSize = alignmentSize * instanceCount;
    device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
  }

  Buffer::~Buffer() {
    unmap();
    vkDestroyBuffer(lveDevice.device(), buffer, nullptr);
    vkFreeMemory(lveDevice.device(), memory, nullptr);
  }

  /**
   * Map a memory range of this buffer. If successful, mapped points to the
   * specified buffer range.
   *
   * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to
   * map the complete buffer range.
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkResult of the buffer mapping call
   */
  VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
    assert(buffer && memory && "Called map on buffer before create");
    return vkMapMemory(lveDevice.device(), memory, offset, size, 0, &mapped);
  }

  /**
   * Unmap a mapped memory range
   *
   * @note Does not return a result as vkUnmapMemory can't fail
   */
  void Buffer::unmap() {
    if (mapped) {
      vkUnmapMemory(lveDevice.device(), memory);
      mapped = nullptr;
    }
  }

  /**
   * Copies the specified data to the mapped buffer. Default value writes whole
   * buffer range
   *
   * @param data Pointer to the data to copy
   * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush
   * the complete buffer range.
   * @param offset (Optional) Byte offset from beginning of mapped region
   *
   */
  void Buffer::writeToBuffer(void *data, const VkDeviceSize size, const VkDeviceSize offset) {
    assert(mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
      memcpy(mapped, data, bufferSize);
    } else {
      auto *memOffset = static_cast<char*>(mapped);
      memOffset += offset;
      memcpy(memOffset, data, size);
    }
  }

  /**
   * Flush a memory range of the buffer to make it visible to the device
   *
   * @note Only required for non-coherent memory
   *
   * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE
   * to flush the complete buffer range.
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkResult of the flush call
   */
  VkResult Buffer::flush(const VkDeviceSize size, const VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange{};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(lveDevice.device(), 1, &mappedRange);
  }

  /**
   * Invalidate a memory range of the buffer to make it visible to the host
   *
   * @note Only required for non-coherent memory
   *
   * @param size (Optional) Size of the memory range to invalidate. Pass
   * VK_WHOLE_SIZE to invalidate the complete buffer range.
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkResult of the invalidate call
   */
  VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(lveDevice.device(), 1, &mappedRange);
  }

  /**
   * Create a buffer info descriptor
   *
   * @param size (Optional) Size of the memory range of the descriptor
   * @param offset (Optional) Byte offset from beginning
   *
   * @return VkDescriptorBufferInfo of specified offset and range
   */
  VkDescriptorBufferInfo Buffer::descriptorInfo(const VkDeviceSize size, const VkDeviceSize offset) {
    return VkDescriptorBufferInfo{
    buffer,
    offset,
    size,
    };
  }

  /**
   * Copies "instanceSize" bytes of data to the mapped buffer at an offset of
   * index * alignmentSize
   *
   * @param data Pointer to the data to copy
   * @param index Used in offset calculation
   *
   */
  void Buffer::writeToIndex(void *data, const int index) {
    writeToBuffer(data, instanceSize, static_cast<VkDeviceSize>(index) * alignmentSize);
  }

  /**
   *  Flush the memory range at index * alignmentSize of the buffer to make it
   * visible to the device
   *
   * @param index Used in offset calculation
   *
   */
  VkResult Buffer::flushIndex(const int index) {
    return flush(alignmentSize, static_cast<VkDeviceSize>(index) * alignmentSize);
  }

  /**
   * Create a buffer info descriptor
   *
   * @param index Specifies the region given by index * alignmentSize
   *
   * @return VkDescriptorBufferInfo for instance at index
   */
  VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(const int index) {
    return descriptorInfo(alignmentSize, static_cast<VkDeviceSize>(index) * alignmentSize);
  }

  /**
   * Invalidate a memory range of the buffer to make it visible to the host
   *
   * @note Only required for non-coherent memory
   *
   * @param index Specifies the region to invalidate: index * alignmentSize
   *
   * @return VkResult of the invalidate call
   */
  VkResult Buffer::invalidateIndex(const int index) {
    return invalidate(alignmentSize, static_cast<VkDeviceSize>(index) * alignmentSize);
  }

} // namespace Enforcer
