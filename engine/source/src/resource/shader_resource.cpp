#include "pd/resource/shader_resource.hpp"

namespace pd {
ShaderResource::ShaderResource(ResourceIdType id, const std::string& name, Backend& backend)
    : Resource(id, name),
      mBackend(&backend) {}

ShaderResource::~ShaderResource() {}

Result<void> ShaderResource::doLoad() noexcept { return {}; }
Result<void> ShaderResource::doUnload() noexcept { return {}; }
}  // namespace pd