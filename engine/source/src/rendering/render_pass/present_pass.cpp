#include "pd/rendering/render_pass/present_pass.hpp"

namespace pd {
void PresentPass::setup() noexcept {}
void PresentPass::execute() noexcept { log::info("present pass execute()"); }
}  // namespace pd