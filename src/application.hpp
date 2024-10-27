#pragma once

#include "enf_descriptors.hpp"
#include "enf_device.hpp"
#include "enf_frame_info.hpp"
#include "enf_game_object.hpp"
#include "enf_renderer.hpp"
#include "enf_texture.hpp"
#include "enf_window.hpp"

#include <memory>
#include <unordered_map>

#define WIDTH 800
#define HEIGHT 600

namespace Enforcer {
class Application {
public:
  explicit Application();
  ~Application();

  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

  void Run();

private:
  void LoadGameObjects();

  void AddTexture(const std::string &filepath) noexcept {
    textures.push_back(
        std::make_unique<Texture>(device, "textures/" + filepath));
  }
  void AddTextureCube(const std::string &filepath) noexcept {
    texturesCube.push_back(
        std::make_unique<Texture>(device, "textures/" + filepath, 6));
  }

  [[deprecated("wip")]] void
  AddObject([[maybe_unused]] const std::string &filepath) noexcept {}

  [[nodiscard]] [[deprecated(
      "not working as expected yet")]] VkDescriptorImageInfo
  GetTextureDescriptorImageInfo(std::unique_ptr<Texture> texture) noexcept {
    VkDescriptorImageInfo textureDescriptorImageInfo{};
    textureDescriptorImageInfo.sampler = texture->GetSampler();
    textureDescriptorImageInfo.imageView = texture->GetImageView();
    textureDescriptorImageInfo.imageLayout = texture->GetImageLayout();
    return textureDescriptorImageInfo;
  }

  template <typename T>
  void UpdateUniformBufferObject(
      const std::vector<std::unique_ptr<Buffer>> &uniformBufferObjectBuffers,
      T &uniformBufferObject) noexcept {
    uniformBufferObjectBuffers[static_cast<u32>(renderer.GetFrameIndex())]
        ->writeToBuffer(&uniformBufferObject);
    uniformBufferObjectBuffers[static_cast<u32>(renderer.GetFrameIndex())]
        ->flush();
  };

  Window window{WIDTH, HEIGHT};
  Device device{window};
  Renderer renderer{window, device};

  // order of declaration matters
  DefaultUniformBufferObject uniformBufferObject;

  std::unique_ptr<DescriptorPool> defaultPool{};
  std::unique_ptr<DescriptorPool> skyboxPool{};

  std::vector<std::unique_ptr<Texture>> textures{};
  std::vector<std::unique_ptr<Texture>> texturesCube{};

  std::unordered_map<u32, GameObject> gameObjects{};
};
} // namespace Enforcer
