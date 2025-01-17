#pragma once

#include "enf_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace Enforcer {

  class DescriptorSetLayout {
  public:
    class Builder {
    public:
      explicit Builder(Device &device) : device{device} {}

      Builder &addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags,
                          uint32_t count = 1);
      std::unique_ptr<DescriptorSetLayout> build() const;

    private:
      Device &device;
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
    };

    DescriptorSetLayout(Device &device, const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> &bindings);
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

  private:
    Device &device;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    friend class DescriptorWriter; // access to private variables
  };

  class DescriptorPool {
  public:
    class Builder {
    public:
      explicit Builder(Device &device) : device{device} {}

      Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
      Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
      Builder &setMaxSets(uint32_t count);
      [[nodiscard]] std::unique_ptr<DescriptorPool> build() const;

    private:
      Device &device;
      std::vector<VkDescriptorPoolSize> poolSizes{};
      uint32_t maxSets{1000};
      VkDescriptorPoolCreateFlags poolFlags = 0;
    };

    DescriptorPool(Device &device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags,
                   const std::vector<VkDescriptorPoolSize> &poolSizes);
    ~DescriptorPool();
    DescriptorPool(const DescriptorPool &) = delete;
    DescriptorPool &operator=(const DescriptorPool &) = delete;

    bool allocateDescriptor(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet> const &descriptors) const;

    void resetPool() const noexcept;

    [[nodiscard]] VkDescriptorPool GetDescriptorPool() const noexcept { return descriptorPool; }

  private:
    Device &device;
    VkDescriptorPool descriptorPool;

    friend class DescriptorWriter; // access to private variables
  };

  class DescriptorWriter {
  public:
    DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPool &pool);

    DescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo const *bufferInfo);
    DescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo const *imageInfo);
    DescriptorWriter &writeImageCube(uint32_t binding, VkDescriptorImageInfo const *imageInfo);
    DescriptorWriter &writeImageArray(u32 binding, VkDescriptorImageInfo const *imageInfo, u32 imageCount);

    bool build(VkDescriptorSet &set);
    void overwrite(VkDescriptorSet const &set) ;

  private:
    DescriptorSetLayout &setLayout;
    DescriptorPool &pool;
    std::vector<VkWriteDescriptorSet> writes;
  };

} // namespace Enforcer
