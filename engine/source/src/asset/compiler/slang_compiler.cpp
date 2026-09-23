#include "pd/asset/compiler/shader_compiler.hpp"

#include "slang.h"
#include "slang-com-ptr.h"
#include "slang-com-helper.h"

namespace pd {
namespace {
void diagnoseIfNeeded(slang::IBlob* diagnosticsBlob) {
  if (diagnosticsBlob != nullptr) {
    LOG_DEBUG(static_cast<const char*>(diagnosticsBlob->getBufferPointer()));
  }
}
}  // namespace
class ShaderCompiler::Impl {
 public:
  Result<ShaderData> compile(const ShaderDesc& spec) noexcept {
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
    sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

    std::array<slang::CompilerOptionEntry, 1> options = {{slang::CompilerOptionName::EmitSpirvDirectly,
                                                          {
                                                              slang::CompilerOptionValueKind::Int,
                                                              1,
                                                              0,
                                                              nullptr,
                                                              nullptr,
                                                          }}};
    sessionDesc.compilerOptionEntries = options.data();
    sessionDesc.compilerOptionEntryCount = options.size();

    Slang::ComPtr<slang::ISession> session;
    globalSession->createSession(sessionDesc, session.writeRef());
    // todo: support #import
    Slang::ComPtr<slang::IModule> slangModule;
    {
      Slang::ComPtr<slang::IBlob> diagnosticsBlob;
      const char* codeData = reinterpret_cast<const char*>(spec.code.data());
      const std::string codeStr{codeData, spec.code.size()};
      slangModule = session->loadModuleFromSourceString(spec.moduleName.c_str(), spec.modulePath.c_str(),
                                                        codeStr.c_str(), diagnosticsBlob.writeRef());
      diagnoseIfNeeded(diagnosticsBlob);
    }
    if (!slangModule) {
      LOG_ERROR("shader compile failed: {}", spec.modulePath);
      return make_error<ShaderData>(ErrorCode::ShaderCompileError);
    }
    // compose
    std::array<slang::IComponentType*, 1> componentTypes = {slangModule};

    Slang::ComPtr<slang::IComponentType> composedProgram;
    {
      Slang::ComPtr<slang::IBlob> diagnosticsBlob;
      SlangResult result = session->createCompositeComponentType(
          componentTypes.data(), componentTypes.size(), composedProgram.writeRef(), diagnosticsBlob.writeRef());
      diagnoseIfNeeded(diagnosticsBlob);
    }
    if (!composedProgram) {
      LOG_ERROR("shader compose failed: {}", spec.modulePath);
      return make_error<ShaderData>(ErrorCode::ShaderCompileError);
    }
    // link
    Slang::ComPtr<slang::IComponentType> linkedProgram;
    {
      Slang::ComPtr<slang::IBlob> diagnosticsBlob;
      SlangResult result = composedProgram->link(linkedProgram.writeRef(), diagnosticsBlob.writeRef());
      diagnoseIfNeeded(diagnosticsBlob);
    }
    if (!linkedProgram) {
      LOG_ERROR("shader link failed: {}", spec.modulePath);
      return make_error<ShaderData>(ErrorCode::ShaderCompileError);
    }
    // terget kernel code
    Slang::ComPtr<slang::IBlob> spirvCode;
    {
      Slang::ComPtr<slang::IBlob> diagnosticsBlob;
      SlangResult result = linkedProgram->getTargetCode(0, spirvCode.writeRef(), diagnosticsBlob.writeRef());
      diagnoseIfNeeded(diagnosticsBlob);
    }
    if (!spirvCode) {
      LOG_ERROR("shader target code failed: {}", spec.modulePath);
      return make_error<ShaderData>(ErrorCode::ShaderCompileError);
    }
    std::vector<u8> spirvCodeData;
    const auto* srcPtr = static_cast<const uint8_t*>(spirvCode->getBufferPointer());
    spirvCodeData.assign(srcPtr, srcPtr + spirvCode->getBufferSize());
    ShaderData data{
        .spirvCode = spirvCodeData,
    };
    return data;
  };
};

ShaderCompiler::ShaderCompiler()
    : mImpl(std::make_unique<ShaderCompiler::Impl>()) {}

ShaderCompiler::~ShaderCompiler() {}

Result<ShaderData> ShaderCompiler::compile(const ShaderDesc& spec) noexcept { return mImpl->compile(spec); }
}  // namespace pd