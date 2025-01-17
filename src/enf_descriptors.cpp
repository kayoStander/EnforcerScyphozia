#include "enf_descriptors.hpp"
#include "../common/logging/log.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace Enforcer {

  // *************** Descriptor Set Layout Builder *********************

  DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::addBinding(u32 binding, VkDescriptorType descriptorType,
                                                                         VkShaderStageFlags stageFlags, u32 count) {
    assert(bindings.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    bindings[binding] = layoutBinding;
    return *this;
  }

  std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const {
    return std::make_unique<DescriptorSetLayout>(device, bindings);
  }

  // *************** Descriptor Set Layout *********************

  DescriptorSetLayout::DescriptorSetLayout(Device &device,
                                           const std::unordered_map<u32, VkDescriptorSetLayoutBinding> &bindings) :
      device{device}, bindings{bindings} {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto kv: bindings) {
      setLayoutBindings.push_back(kv.second);
    } // [const unsigned i, VkDescriptorSetLayoutBinding descriptorSetLayoutBinding]

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<u32>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(device.device(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create descriptor set layout!");
    }
  }

  DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(device.device(), descriptorSetLayout, nullptr);
  }

  // *************** Descriptor Pool Builder *********************

  DescriptorPool::Builder &DescriptorPool::Builder::addPoolSize(const VkDescriptorType descriptorType,
                                                                const u32 count) {
    poolSizes.push_back({descriptorType, count});
    return *this;
  }

  DescriptorPool::Builder &DescriptorPool::Builder::setPoolFlags(const VkDescriptorPoolCreateFlags flags) {
    poolFlags = flags;
    return *this;
  }
  DescriptorPool::Builder &DescriptorPool::Builder::setMaxSets(const u32 count) {
    maxSets = count;
    return *this;
  }

  std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
    return std::make_unique<DescriptorPool>(device, maxSets, poolFlags, poolSizes);
  }

  // *************** Descriptor Pool *********************

  DescriptorPool::DescriptorPool(Device &device, const u32 maxSets, const VkDescriptorPoolCreateFlags poolFlags,
                                 const std::vector<VkDescriptorPoolSize> &poolSizes) :
      device{device}, descriptorPool{} {
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<u32>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    if (vkCreateDescriptorPool(device.device(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
      throw std::runtime_error("failed to create descriptor pool!");
    }
  }

  DescriptorPool::~DescriptorPool() { vkDestroyDescriptorPool(device.device(), descriptorPool, nullptr); }

  bool DescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout,
                                          VkDescriptorSet &descriptor) const {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this
    // case, and builds a new pool whenever an old pool fills up. But this is
    // beyond our current scope
    if (vkAllocateDescriptorSets(device.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
      return false;
    }
    return true;
  }

  void DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> const &descriptors) const {
    vkFreeDescriptorSets(device.device(), descriptorPool, static_cast<u32>(descriptors.size()), descriptors.data());
  }

  void DescriptorPool::resetPool() const noexcept { vkResetDescriptorPool(device.device(), descriptorPool, 0); }

  // *************** Descriptor Writer *********************

  DescriptorWriter::DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPool &pool) :
      setLayout{setLayout}, pool{pool} {}

  DescriptorWriter &DescriptorWriter::writeBuffer(const u32 binding, VkDescriptorBufferInfo const *bufferInfo) {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = setLayout.bindings[binding];

    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
  }

  DescriptorWriter &DescriptorWriter::writeImage(const u32 binding, VkDescriptorImageInfo const *imageInfo) {
    LOG_INFO(Vulkan, "Bindings => {}", setLayout.bindings.count(binding));
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    LOG_INFO(Vulkan, "jayce");

    auto &bindingDescription = setLayout.bindings[binding];

    LOG_INFO(Vulkan, "jaybe");

    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
  }

  DescriptorWriter &DescriptorWriter::writeImageCube(const u32 binding, VkDescriptorImageInfo const *imageInfo) {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = setLayout.bindings[binding];

    assert(bindingDescription.descriptorCount < 6 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 6;

    writes.push_back(write);
    return *this;
  }

  DescriptorWriter &DescriptorWriter::writeImageArray(const u32 binding, VkDescriptorImageInfo const *imageInfo,
                                                      const u32 imageCount) {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = setLayout.bindings[binding];

    assert(bindingDescription.descriptorCount >= imageCount && "Binding expects more descriptors than provided");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = imageCount;

    writes.push_back(write);
    return *this;
  }

  bool DescriptorWriter::build(VkDescriptorSet &set) {
    if (const bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set); !success) {
      return false;
    }
    overwrite(set);
    return true;
  }

  void DescriptorWriter::overwrite(VkDescriptorSet const &set) {
    for (auto &write: writes) {
      write.dstSet = set;
    }
    vkUpdateDescriptorSets(pool.device.device(), writes.size(), writes.data(), 0, nullptr);
  }

} // namespace Enforcer
