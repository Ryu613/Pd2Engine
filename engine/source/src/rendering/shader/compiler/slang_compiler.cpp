#include "pd/rendering/shader/shader_compiler.hpp"

#include "slang.h"
#include "slang-com-ptr.h"
#include "slang-com-helper.h"

namespace pd {
class ShaderCompiler::Impl {
 public:
  Result<ShaderData> compile(const ShaderSpec& spec) noexcept {
    // slang compilation
    Slang::ComPtr<slang::IGlobalSession> globalSession;
    slang::createGlobalSession(globalSession.writeRef());
    slang::SessionDesc sessionDesc{};
    slang::TargetDesc targetDesc{
        .format = SLANG_SPIRV,
        .profile = globalSession->findProfile("spirv_1_5"),
    };
    sessionDesc.targets = &targetDesc;
    sessionDesc.targetCount = 1;

    Slang::ComPtr<slang::ISession> session;
    globalSession->createSession(sessionDesc, session.writeRef());
    // session->loadModuleFromSource(spec.moduleName.c_str(), spec.modulePath.c_str(), spec.code);
    return {};
  };
};

ShaderCompiler::ShaderCompiler()
    : mImpl(std::make_unique<ShaderCompiler::Impl>()) {}

ShaderCompiler::~ShaderCompiler() {}

Result<ShaderData> ShaderCompiler::compile(const ShaderSpec& spec) noexcept { return mImpl->compile(spec); }
}  // namespace pd