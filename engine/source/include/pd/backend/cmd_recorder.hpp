#pragma once

#include "pd/backend/backend_types.hpp"

namespace pd {
struct CommandPayload {
  CmdType type;
  std::array<u64, 10> args;
};
template <typename T>
concept CmdArgsRequire =
    std::is_trivially_copyable_v<T> &&            // 1. 必须是平凡可拷贝
    (sizeof(T) <= sizeof(CommandPayload::args));  // 2. 大小不能超过 80 字节(sizeof(u64) * 10)
class CommandRecorder {
 public:
  CommandRecorder() { mCmds.reserve(4096); }
  ~CommandRecorder() = default;

  template <CmdArgsRequire CmdArgs>
  void recordCmd(const CmdArgs& cmdArgs) {
    CommandPayload payload{
        .type = CmdArgs::type,
    };

    std::memcpy(&payload.args[0], &cmdArgs, sizeof(CmdArgs));

    mCmds.push_back(payload);
  }

  void submit() {}

  void reset() { mCmds.clear(); }

 private:
  std::vector<CommandPayload> mCmds;
};
}  // namespace pd