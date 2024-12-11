#pragma once

#include "enf_descriptors.hpp"
#include "enf_device.hpp"
#include "enf_frame_info.hpp"
#include "enf_game_object.hpp"
#include "enf_renderer.hpp"
#include "enf_texture.hpp"
#include "enf_window.hpp"
#include "../game/game_player.hpp"
#include "enf_server.hpp"

#include <memory>
#include <unordered_map>

#define WIDTH 800
#define HEIGHT 600

namespace Enforcer {
  class Application {
  public:
    explicit Application();
    ~Application() = default;

    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    void Run();

  private:
    void LoadGameObjects();

    void AddTexture(const std::string &filepath) noexcept {
      textures.push_back(std::make_unique<Texture>(device, "textures/" + filepath));
    }
    void AddTextureCube(const std::string &filepath) noexcept {
      texturesCube.push_back(std::make_unique<Texture>(device, "textures/" + filepath, 6));
    }

    GameObject CreateObject() noexcept {
      GameObject gameObject{GameObject::CreateGameObject()};
      gameObjects.emplace(gameObject.GetId(), std::move(gameObject));
      return gameObject;
    }

    [[nodiscard]] [[deprecated(
    "not working as expected yet")]] VkDescriptorImageInfo static GetTextureDescriptorImageInfo(const std::
                                                                                                unique_ptr<Texture>
                                                                                                &texture) noexcept {
      VkDescriptorImageInfo textureDescriptorImageInfo{};
      textureDescriptorImageInfo.sampler = texture->GetSampler();
      textureDescriptorImageInfo.imageView = texture->GetImageView();
      textureDescriptorImageInfo.imageLayout = texture->GetImageLayout();
      return textureDescriptorImageInfo;
    }

    template<typename T>
    void UpdateUniformBufferObject(const std::vector<std::unique_ptr<Buffer>> &uniformBufferObjectBuffers,
                                   T &uniformBufferObject) noexcept {
      uniformBufferObjectBuffers[renderer.GetFrameIndex()]->writeToBuffer(&uniformBufferObject);
      uniformBufferObjectBuffers[renderer.GetFrameIndex()]->flush();
    };


    /*class CustomServer : public ServerInterface<CustomMessageTypes> {
    public:
      CustomServer(const u16 port):ServerInterface(port) {

      }
    protected:
      virtual bool OnClientConnect([[maybe_unused]]std::shared_ptr<Connection<CustomMessageTypes>> client) {
        return true;
      }
      virtual void OnClientDisconnect([[maybe_unused]]std::shared_ptr<Connection<CustomMessageTypes>> client) {

      }
      virtual void OnMessage(const std::shared_ptr<Connection<CustomMessageTypes>> client, const Message<CustomMessageTypes>& message) {
        switch (message.header.id) {
          case CustomMessageTypes::ServerPing: {
            LOG_INFO(Server,"[{}] ServerPing()", client->GetId());
            client->Send(message);
            break;
          }
          default: {break;}
        }
      }
    } server{60000};*/

    Window window{WIDTH, HEIGHT};
    Device device{window};
    Renderer renderer{window, device};

    // order of declaration matters
    DefaultUniformBufferObject uniformBufferObject;

    //std::unique_ptr<Buffer> imGuiVertexBuffer{std::make_unique<Buffer>(device,)};
    //std::unique_ptr<Buffer> imGuiIndexBuffer;

    std::unique_ptr<DescriptorPool> defaultPool{};
    std::unique_ptr<DescriptorPool> imGuiPool{};

    std::vector<std::unique_ptr<Texture>> textures{};
    std::vector<std::unique_ptr<Texture>> texturesCube{};

    std::unordered_map<u32, GameObject> gameObjects{};
  };
} // namespace Enforcer
