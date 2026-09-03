#pragma once

#include "pd/backend/hw_enums.hpp"
#include "pd/backend/hw_resource.hpp"
#include "pd/backend/hw_handle.hpp"

namespace pd {
struct Buffer_t;
/**
 * @brief gpu层texture表示,纯结构体，只用于记录跨backend的基本信息
 *
 */
struct BufferWriteOptions {
  HwHandle<Buffer_t> dstBuffer;
  uint64_t byteSize = 0;
  const void* pData = nullptr;
  uint64_t dstOffset = 0;
};
struct HwBuffer : public HwResource {
  BufferUsage usage = BufferUsage::VertexBuffer;
  SharingMode sharingMode = SharingMode::Exclusive;
  MemoryUsage memoryUsage = MemoryUsage::GpuOnly;
  uint64_t deviceSize = 0;
};
}  // namespace pd