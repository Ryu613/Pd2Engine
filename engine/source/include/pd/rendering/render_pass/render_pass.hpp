#pragma once

namespace pd {

class RenderPass {
 public:
  RenderPass() noexcept = default;
  virtual ~RenderPass() = default;
  MOVABLE_ONLY(RenderPass);

  virtual void setup() noexcept = 0;
  virtual void execute() noexcept = 0;

 protected:
  // std::vector<RgHandle> inputs;
  // std::vector<RgHandle> outputs;
};

struct PassNode {
  std::string name;
  uint32_t id = UINT32_MAX;
  RenderPass* pNext = nullptr;
  RenderPass* pPass = nullptr;
};
}  // namespace pd