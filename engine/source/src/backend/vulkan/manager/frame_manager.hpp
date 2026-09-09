#pragma once

#include "pd/backend/command_recorder.hpp"

#include "../vk1_device.hpp"
#include "../vk1_helper.hpp"

#include <array>

namespace vk1 {
struct FrameData {
  uint32_t frameIndex = UINT32_MAX;
  uint32_t swapchainImageIndex = UINT32_MAX;
};
class FrameManager {
 public:
  inline static constexpr size_t FrameCount = global::maxInflightFrames;

  explicit FrameManager(Vk1Device& device)
      : mDevice(&device) {}

  ~FrameManager() = default;
  FrameManager(const FrameManager&) = delete;
  FrameManager& operator=(const FrameManager&) = delete;
  FrameManager(FrameManager&& rhs) noexcept = delete;
  FrameManager& operator=(FrameManager&& rhs) noexcept = delete;

  void init() noexcept;
  void destroy() noexcept;

  FrameData beginFrame() noexcept;
  void endFrame(const pd::CommandRecorder recorder) noexcept;

 private:
  struct Frame {
    void init(Vk1Device& device, size_t index);
    void destroy();

    void setUsing(bool flag) noexcept { isUsing = flag; }

    Vk1Device* pDevice = nullptr;
    bool isUsing = false;
    size_t frameIndex = 0;
    VkFence frameFence = VK_NULL_HANDLE;
    VkSemaphore acquireImageSemaphore = VK_NULL_HANDLE;
    VkCommandPool cmdPool = VK_NULL_HANDLE;
    VkCommandBuffer mainCmdBuffer = VK_NULL_HANDLE;
    Vk1Image depthImage{};
    Vk1ImageView depthImageView{};
  };

  Vk1Device* mDevice = nullptr;
  std::array<Frame, FrameCount> mFrames;
  size_t mCurrentFrameIndex = 0;

  void replayCommands(const pd::CommandRecorder& recorder) noexcept;
  void replayCmd(const pd::CommandPayload& payload, uint32_t frameIndex, uint32_t imageIndex) noexcept;
  void advanceFrameIndex(uint32_t currentFrameIndex) noexcept;
};

}  // namespace vk1