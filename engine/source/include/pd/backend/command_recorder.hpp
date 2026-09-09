#pragma once

#include "pd/backend/backend_types.hpp"

namespace pd {
template <typename T>
concept CmdArgsRequire =
    std::is_trivially_copyable_v<T> &&            // 1. 必须是平凡可拷贝
    (sizeof(T) <= sizeof(CommandPayload::args));  // 2. 大小不能超过 80 字节(sizeof(u64) * 10)
class CommandRecorder {
 public:
  struct Info {
    u32 frameIndex = u32_max;
    u32 swapchainImageIndex = u32_max;
  };
  CommandRecorder() { mCmds.reserve(4096); }
  ~CommandRecorder() = default;

  template <CmdArgsRequire CmdArgs>
  void addCmd(const CmdArgs& cmdArgs) {
    CommandPayload payload{
        .type = CmdArgs::type,
    };

    std::memcpy(&payload.args[0], &cmdArgs, sizeof(CmdArgs));

    mCmds.push_back(payload);
  }

  void clear() { mCmds.clear(); }

  const auto& getCommands() const noexcept { return mCmds; }

  void setInfo(u32 frameIndex, u32 swapchainIndex) noexcept {
    mInfo.frameIndex = frameIndex;
    mInfo.swapchainImageIndex = swapchainIndex;
  }

  const Info& getInfo() const noexcept { return mInfo; }

 private:
  Info mInfo;
  std::vector<CommandPayload> mCmds;
};
}  // namespace pd