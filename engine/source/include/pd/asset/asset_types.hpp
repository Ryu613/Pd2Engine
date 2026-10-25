#pragma once

#include "pd/backend/backend_types.hpp"

#include <variant>
#include <algorithm>

namespace pd {
enum class ParseType : u8 {
  Gltf,
  Shader,
};

enum class MaterialType : u8 {
  PbrMetallicRoughness,
};

enum class TextureDecoderType : u8 {
  Stb,
  BasisU,
  Dds,
  WebP,
  Custom,
};

enum class TextureMimeType : u8 {
  Unknown = 0,
  ImagePng,
  ImageJpeg,
  ImageKtx2,
  ImageDds,
  ImageWebp,
  ApplicationGltfBuffer,
  ApplicationOctetStream,
};

using AssetIdType = u64;
inline constexpr AssetIdType invalidAssetId = std::numeric_limits<AssetIdType>::max();

inline constexpr u32 invalidAssetIndex = std::numeric_limits<u32>::max();

using AssetPathType = std::string;
using VertexData = pd::Vertex;

struct AssetHandle {
  AssetIdType id = invalidAssetId;
};

struct ImageData {
  AssetIdType id;
  std::vector<std::byte> bytes;
};
struct SamplerData {
  // todo
};
struct TextureData {
  AssetIdType id;
  std::string name;
  TextureMimeType mimeType;
  struct DataUri {
    std::string uri;
  };
  struct ExternalFile {
    std::string uri;
  };
  struct BufferView {
    const std::byte* dataPtr = nullptr;
    size_t byteLength = 0;
  };
  u32 imageIndex = invalidAssetIndex;
  u32 samplerIndex = invalidAssetIndex;
  std::variant<DataUri, ExternalFile, BufferView> source;
  TextureDecoderType decoderType = TextureDecoderType::Stb;
  u32 width = 0;
  u32 height = 0;
  u32 channels = 0;
  u32 mipLevels = 1;
};

inline std::string_view toString(TextureMimeType mime) {
  switch (mime) {
    case TextureMimeType::ImagePng:
      return "image/png";
    case TextureMimeType::ImageJpeg:
      return "image/jpeg";
    case TextureMimeType::ImageKtx2:
      return "image/ktx2";
    case TextureMimeType::ImageDds:
      return "image/vnd-ms.dds";
    case TextureMimeType::ImageWebp:
      return "image/webp";
    case TextureMimeType::ApplicationGltfBuffer:
      return "application/gltf-buffer";
    case TextureMimeType::ApplicationOctetStream:
      return "application/octet-stream";
    case TextureMimeType::Unknown:
      return "";
  }
  return "";
}

inline TextureMimeType TextureMimeTypefromExtensionName(std::string_view str) {
  static constexpr std::array<std::pair<std::string_view, TextureMimeType>, 6> extTable{{
      {".png", TextureMimeType::ImagePng},
      {".jpg", TextureMimeType::ImageJpeg},
      {".jpeg", TextureMimeType::ImageJpeg},
      {".ktx2", TextureMimeType::ImageKtx2},
      {".dds", TextureMimeType::ImageDds},
      {".webp", TextureMimeType::ImageWebp},
  }};
  std::string lower{str};
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
  for (auto [e, t] : extTable) {
    if (e == lower) return t;
  }

  return TextureMimeType::Unknown;
}

inline AssetIdType getNewAssetId() noexcept {
  static std::atomic<AssetIdType> newId{0};
  return ++newId;
}

}  // namespace pd