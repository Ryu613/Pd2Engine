#include "Resource_registry.hpp"

#include "../backend_vulkan_types.hpp"

namespace vk1 {
ResourceRegistry::ResourceRegistry(Vk1Device& device)
    : mDevice(&device) {}

ResourceRegistry::~ResourceRegistry() {}

void ResourceRegistry::init() noexcept {}
void ResourceRegistry::destroy() noexcept {
  // todo: destroy all dataPool handles
}

pd::HwShaderModuleHandle ResourceRegistry::createShaderModule(
    const pd::ShaderModuleCreateDesc& shaderModuleCreateDesc) noexcept {
  // create shader module
  VkShaderModuleCreateInfo moduleInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = shaderModuleCreateDesc.spirCode.size(),
      .pCode = reinterpret_cast<const uint32_t*>(shaderModuleCreateDesc.spirCode.data()),
  };
  VkShaderModule shaderModule = 0;
  checkResult(vkCreateShaderModule(mDevice->getDevice(), &moduleInfo, 0, &shaderModule));
  assert(shaderModule);

  Slot<pd::ShaderModule_t, Vk1ShaderModule> slot{
      .gen = 0,
      .resource =
          {
              .handle = shaderModule,
          },
  };
  auto newId = nextId<pd::ShaderModule_t>();
  mShaderModules.emplace(newId, slot);

  setObjectName(mDevice->getDevice(), VK_OBJECT_TYPE_SHADER_MODULE, shaderModule, shaderModuleCreateDesc.debugName);

  return {.data = {
              .id = newId,
              .gen = 0,
          }};
}
void ResourceRegistry::destroyShaderModule(pd::HwShaderModuleHandle handle) noexcept {
  const auto* shaderModule = getResource(handle);
  if (shaderModule->handle != VK_NULL_HANDLE) {
    vkDestroyShaderModule(mDevice->getDevice(), shaderModule->handle, 0);
  }
  destroyResource(handle);
}

pd::HwBindingLayoutHandle ResourceRegistry::createBindingLayout(const pd::BindingLayoutCreateDesc& desc) noexcept {
  auto vkDevice = mDevice->getDevice();

  std::vector<VkDescriptorSetLayoutBinding> bindings;
  for (const auto& binding : desc.bindings) {
    VkDescriptorSetLayoutBinding newBinding{
        .binding = binding.slot,
        .descriptorType = util::toVkDescriptorType(binding.resourceType),
        .descriptorCount = binding.count,
        .stageFlags = util::toVkShaderStageFlagBits(binding.shaderStages),
        .pImmutableSamplers = 0,
    };
    // todo: immutable samplers

    bindings.emplace_back(newBinding);
  }

  VkDescriptorSetLayoutCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = static_cast<uint32_t>(bindings.size()),
      .pBindings = bindings.data(),
  };

  if (desc.bindless) {
    createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

    // 允许每个 Binding 中没有被 Shader 实际使用的 Descriptor 保持未初始化
    // 必须已经启用 descriptorBindingPartiallyBound 功能
    std::vector<VkDescriptorBindingFlags> bindingFlags(bindings.size(), VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

    for (size_t i = 0; i < bindings.size(); ++i) {
      bindingFlags[i] |=
          VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
    }

    VkDescriptorSetLayoutBindingFlagsCreateInfo nextInfo{
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindingFlags = bindingFlags.data(),
    };

    createInfo.pNext = &nextInfo;
  }

  VkDescriptorSetLayout setLayout = 0;
  checkResult(vkCreateDescriptorSetLayout(vkDevice, &createInfo, 0, &setLayout));
  assert(setLayout);

  Slot<pd::BindingLayout_t, Vk1BindingLayout> slot{
      .gen = 0,
      .resource =
          {
              .handle = setLayout,
              .bindings = bindings,
          },
  };
  auto newId = nextId<pd::BindingLayout_t>();
  mBindingLayouts.emplace(newId, slot);

  setObjectName(mDevice->getDevice(), VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, setLayout, desc.debugName);

  return {.data = {
              .id = newId,
              .gen = 0,
          }};
}

void ResourceRegistry::destroyBindingLayout(pd::HwBindingLayoutHandle handle) noexcept {
  if (!handle) {
    return;
  }
  const auto& vkDevice = mDevice->getDevice();
  auto setLayout = getResource(handle);
  assert(setLayout->handle);

  vkDestroyDescriptorSetLayout(vkDevice, setLayout->handle, 0);

  destroyResource(handle);
}

pd::HwPipelineLayoutHandle ResourceRegistry::createPipelineLayout(const pd::PipelineLayoutCreateDesc& desc) noexcept {
  VkPipelineLayoutCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
  };
  // push constants
  std::vector<VkPushConstantRange> vkPushConstantRanges;
  if (!desc.pushConstantRanges.empty()) {
    vkPushConstantRanges.reserve(desc.pushConstantRanges.size());
    for (auto& pushconstantRange : desc.pushConstantRanges) {
      VkPushConstantRange vkPushConstantRange{
          .stageFlags = util::toVkShaderStageFlagBits(pushconstantRange.shaderStages),
          .offset = pushconstantRange.offset,
          .size = pushconstantRange.size,
      };
      vkPushConstantRanges.emplace_back(vkPushConstantRange);
    }
  }
  createInfo.pushConstantRangeCount = static_cast<uint32_t>(vkPushConstantRanges.size());
  createInfo.pPushConstantRanges = vkPushConstantRanges.data();

  // descriptor set layouts
  std::vector<VkDescriptorSetLayout> setLayouts;
  if (!desc.bindGroupLayouts.empty()) {
    for (const auto& setLayout : desc.bindGroupLayouts) {
      const auto* resource = getResource(setLayout);
      assert(resource);
      setLayouts.push_back(resource->handle);
    }
  }
  createInfo.setLayoutCount = setLayouts.size();
  createInfo.pSetLayouts = setLayouts.data();

  VkPipelineLayout layout = 0;
  checkResult(vkCreatePipelineLayout(mDevice->getDevice(), &createInfo, 0, &layout));
  assert(layout);
  Slot<pd::PipelineLayout_t, Vk1PipelineLayout> slot{
      .gen = 0,
      .resource =
          {
              .handle = layout,
          },
  };
  auto newId = nextId<pd::PipelineLayout_t>();
  mPipelineLayouts.emplace(newId, slot);

  setObjectName(mDevice->getDevice(), VK_OBJECT_TYPE_PIPELINE_LAYOUT, layout, desc.debugName);

  return {.data = {
              .id = newId,
              .gen = 0,
          }};
}

void ResourceRegistry::destroyPipelineLayout(pd::HwPipelineLayoutHandle handle) noexcept {
  if (!handle) {
    return;
  }
  const auto& vkDevice = mDevice->getDevice();
  auto layout = getResource(handle);
  assert(layout->handle);

  vkDestroyPipelineLayout(vkDevice, layout->handle, 0);

  destroyResource(handle);
}

pd::HwGraphicsPipelineHandle ResourceRegistry::createGraphicsPipeline(
    const pd::GraphicsPipelineCreateDesc& desc) noexcept {
  const auto& vkDevice = mDevice->getDevice();
  // get pipeline layout
  const auto* vk1PipelineLayout = getResource(desc.layout);
  if (vk1PipelineLayout == nullptr) {
    return {};
  }
  //   compile shader modules
  std::vector<VkPipelineShaderStageCreateInfo> stages;
  stages.resize(desc.shaderPrograms.size());
  for (size_t i = 0; i < desc.shaderPrograms.size(); ++i) {
    const auto& shaderProgram = desc.shaderPrograms[i];
    const auto& shaderModuleHandle = desc.shaderModules[shaderProgram.shaderModuleIndex];
    const auto* shaderModule = this->getResource(shaderModuleHandle);
    assert(shaderModule->handle);
    stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[i].stage = util::toVkShaderStageFlagBits(shaderProgram.stage);
    stages[i].module = shaderModule->handle;
    stages[i].pName = shaderProgram.entryPoint.data();
  };

  std::vector<VkVertexInputAttributeDescription> vertexAttrs;
  vertexAttrs.reserve(desc.vertexOptions.attributes.size());

  for (const auto& vAttr : desc.vertexOptions.attributes) {
    vertexAttrs.emplace_back(VkVertexInputAttributeDescription{
        .location = vAttr.location,
        .binding = vAttr.binding,
        .format = util::toVkFormat(vAttr.format),
        .offset = vAttr.offset,
    });
  }

  std::vector<VkVertexInputBindingDescription> vertexBindings;
  vertexBindings.reserve(desc.vertexOptions.vertexLayouts.size());
  for (const auto& vLayout : desc.vertexOptions.vertexLayouts) {
    vertexBindings.emplace_back(VkVertexInputBindingDescription{
        .binding = vLayout.binding,
        .stride = vLayout.stride,
        .inputRate = util::toVkInputRate(vLayout.inputRate),
    });
  }

  VkPipelineVertexInputStateCreateInfo vertexInput{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size()),
      .pVertexBindingDescriptions = vertexBindings.data(),
      .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttrs.size()),
      .pVertexAttributeDescriptions = vertexAttrs.data(),
  };

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
  };
  VkPipelineViewportStateCreateInfo viewportState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .scissorCount = 1,
  };
  VkPipelineRasterizationStateCreateInfo rasterization{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = util::toVkCullMode(desc.primitiveOptions.cullMode),
      .frontFace = util::toVkFrontFace(desc.primitiveOptions.frontFace),
      .lineWidth = 1.0f,
  };
  VkPipelineColorBlendAttachmentState colorAttachment{
      .colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };
  VkPipelineColorBlendStateCreateInfo colorBlend{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments = &colorAttachment,
  };
  std::vector<VkDynamicState> dynamicStates = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
  };
  VkPipelineDynamicStateCreateInfo dynamicState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = 2,
      .pDynamicStates = dynamicStates.data(),
  };
  VkPipelineMultisampleStateCreateInfo multisampleState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
  };

  VkPipelineDepthStencilStateCreateInfo depthStencil{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      .depthTestEnable = VK_TRUE,
      .depthWriteEnable = VK_TRUE,
      .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
  };

  auto imageFormat = mDevice->getSwapchainInfo().format;

  VkPipelineRenderingCreateInfo renderingInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
      .colorAttachmentCount = 1,
      .pColorAttachmentFormats = &imageFormat,
      .depthAttachmentFormat = VK_FORMAT_D32_SFLOAT,  // fixed value for now
  };

  VkGraphicsPipelineCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = &renderingInfo,
      .stageCount = static_cast<uint32_t>(stages.size()),
      .pStages = stages.data(),
      .pVertexInputState = &vertexInput,
      .pInputAssemblyState = &inputAssembly,
      .pViewportState = &viewportState,
      .pRasterizationState = &rasterization,
      .pMultisampleState = &multisampleState,
      .pDepthStencilState = &depthStencil,
      .pColorBlendState = &colorBlend,
      .pDynamicState = &dynamicState,
      .layout = vk1PipelineLayout->handle,
  };

  VkPipeline pipeline = 0;
  checkResult(vkCreateGraphicsPipelines(vkDevice, 0, 1, &createInfo, 0, &pipeline));
  assert(pipeline);

  Slot<pd::GraphicsPipeline_t, Vk1Pipeline> slot{
      .gen = 0,
      .resource =
          {
              .layout = vk1PipelineLayout->handle,
              .handle = pipeline,
          },
  };
  auto newId = nextId<pd::GraphicsPipeline_t>();
  mGraphicsPipelines.emplace(newId, slot);

  setObjectName(mDevice->getDevice(), VK_OBJECT_TYPE_PIPELINE, pipeline, desc.debugName);

  return {.data = {
              .id = newId,
              .gen = 0,
          }};
}

void ResourceRegistry::destroyGraphicsPipeline(pd::HwGraphicsPipelineHandle handle) noexcept {
  if (!handle) {
    return;
  }
  auto vkDevice = mDevice->getDevice();
  auto pipeline = getResource(handle);
  assert(pipeline->handle);

  vkDestroyPipeline(vkDevice, pipeline->handle, 0);

  destroyResource(handle);
}

pd::HwBufferHandle ResourceRegistry::createBuffer(const pd::BufferCreateDesc& desc) noexcept {
  const auto vkDevice = mDevice->getDevice();
  VkBufferCreateInfo bufferCreateInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = desc.deviceSize,
      .usage = util::ToVkBufferUsage(desc.usage),
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };
  VmaAllocationCreateInfo allocInfo{
      .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
               VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
      .usage = VMA_MEMORY_USAGE_AUTO,
  };
  Vk1Buffer vk1Buffer;
  checkResult(vmaCreateBuffer(mDevice->getAllocator(), &bufferCreateInfo, &allocInfo, &vk1Buffer.handle,
                              &vk1Buffer.allocation, &vk1Buffer.allocationInfo));
  assert(vk1Buffer.handle);

  if (desc.usage == pd::BufferUsage::UniformBuffer) {
    VkBufferDeviceAddressInfo bufferBdaInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .buffer = vk1Buffer.handle,
    };
    // able to access the buffer in shader
    vk1Buffer.deviceAddress = vkGetBufferDeviceAddress(vkDevice, &bufferBdaInfo);
  }

  Slot<pd::Buffer_t, Vk1Buffer> slot{
      .gen = 0,
      .resource = vk1Buffer,
  };
  auto newId = nextId<pd::Buffer_t>();
  mBuffers.emplace(newId, slot);

  setObjectName(vkDevice, VK_OBJECT_TYPE_BUFFER, vk1Buffer.handle, desc.debugName);

  return {{
      newId,
      0,
  }};
}

void ResourceRegistry::writeBuffer(const pd::BufferWriteDesc& desc) noexcept {
  auto* buffer = getResource(desc.buffer);
  assert(buffer->handle);
  memcpy(buffer->allocationInfo.pMappedData, desc.pData, desc.deviceSize);
}

void ResourceRegistry::destroyBuffer(pd::HwBufferHandle buffer) noexcept {
  auto* vk1Buffer = getResource(buffer);
  assert(vk1Buffer->handle);
  vmaDestroyBuffer(mDevice->getAllocator(), vk1Buffer->handle, vk1Buffer->allocation);

  destroyResource(buffer);
}

pd::HwTextureHandle ResourceRegistry::createTexture(const pd::TextureCreateDesc& textureCreateDesc) noexcept {
  const auto vkDevice = mDevice->getDevice();
  VkExtent3D extent = {
      .width = textureCreateDesc.extent.width,
      .height = textureCreateDesc.extent.height,
      .depth = textureCreateDesc.extent.depth,
  };
  VkImageCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .format = util::toVkFormat(textureCreateDesc.format),
      .extent = extent,
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = util::toVkImageUsage(textureCreateDesc.usage),
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };
  VmaAllocationCreateInfo allocCI{
      .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
      .usage = VMA_MEMORY_USAGE_AUTO,
  };
  Vk1Image image{
      .format = createInfo.format,
      .extent = createInfo.extent,
      .mipLevels = createInfo.mipLevels,
      .arrayLayers = createInfo.arrayLayers,
  };
  checkResult(vmaCreateImage(mDevice->getAllocator(), &createInfo, &allocCI, &image.handle, &image.allocation,
                             &image.allocationInfo));
  assert(image.handle);

  Slot<pd::Texture_t, Vk1Image> slot{
      .gen = 0,
      .resource = image,
  };
  auto newId = nextId<pd::Texture_t>();
  mImages.emplace(newId, slot);

  setObjectName(vkDevice, VK_OBJECT_TYPE_IMAGE, image.handle, textureCreateDesc.debugName);

  return {{
      newId,
      0,
  }};
}

void ResourceRegistry::destroyTexture(pd::HwTextureHandle handle) noexcept {
  auto* vk1Image = getResource(handle);
  assert(vk1Image->handle);
  vmaDestroyImage(mDevice->getAllocator(), vk1Image->handle, vk1Image->allocation);

  destroyResource(handle);
}
}  // namespace vk1