#pragma once

#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_enums.hpp"
#include "pd/backend/hw_resource.hpp"

namespace pd {

struct Swapchain_t;

struct HwSwapchain : public HwResource {
  TextureFormat format = TextureFormat::RGBA8SRGB;
  ColorSpace colorSpace = ColorSpace::SrgbNonLinear;
  PresentMode presentMode = PresentMode::Fifo;
  TextureUsage imageUsageFlags = TextureUsage::ColorAttachment;
  Extent extent;
  HwHandle<Swapchain_t> oldSwapchain;
};
}  // namespace pd