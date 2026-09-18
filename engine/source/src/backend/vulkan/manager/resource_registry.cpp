#include "Resource_registry.hpp"

#include "../backend_vulkan_types.hpp"

namespace vk1 {
ResourceRegistry::ResourceRegistry(Vk1Device& device)
    : mDevice(&device) {}

ResourceRegistry::~ResourceRegistry() {}

void ResourceRegistry::init() noexcept {}
void ResourceRegistry::destroy() noexcept {}

pd::HwPipelineLayoutHandle ResourceRegistry::createPipelineLayout(const pd::PipelineLayoutDesc& desc) noexcept {
  VkPipelineLayoutCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 0,
      .pSetLayouts = 0,
  };
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
pd::HwGraphicsPipelineHandle ResourceRegistry::createGraphicsPipeline(pd::HwPipelineLayoutHandle layoutHandle,
                                                                      const pd::GraphicsPipelineDesc& desc) noexcept {
  const auto& vkDevice = mDevice->getDevice();
  // get pipeline layout
  const auto* vk1PipelineLayout = getResource(layoutHandle);
  if (vk1PipelineLayout == nullptr) {
    return {};
  }
  //   compile shader modules
  std::vector<VkPipelineShaderStageCreateInfo> stages;
  stages.resize(desc.shaderPrograms.size());
  for (size_t i = 0; i < desc.shaderPrograms.size(); ++i) {
    const auto& shaderProgram = desc.shaderPrograms[i];
    const auto& shaderCode = desc.shaderDatas[shaderProgram.shaderCodeIndex].spirvCode;
    // create shader module
    VkShaderModuleCreateInfo moduleInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = shaderCode.size(),
        .pCode = reinterpret_cast<const uint32_t*>(shaderCode.data()),
    };
    VkShaderModule shaderModule = 0;
    checkResult(vkCreateShaderModule(vkDevice, &moduleInfo, 0, &shaderModule));
    assert(shaderModule);
    stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[i].stage = util::toVkShaderStageFlagBits(shaderProgram.stage);
    stages[i].module = shaderModule;
    stages[i].pName = shaderProgram.entryPoint.data();
  };

  // std::array<VkVertexInputAttributeDescription, 3> vertexAttrs;
  // vertexAttrs[0] = {
  //     .location = 0,
  //     .binding = 0,
  //     .format = VK_FORMAT_R32G32B32_SFLOAT,
  //     .offset = offsetof(Vertex, position),
  // };
  // vertexAttrs[1] = {
  //     .location = 1,
  //     .binding = 0,
  //     .format = VK_FORMAT_R32G32B32_SFLOAT,
  //     .offset = offsetof(Vertex, color),
  // };
  // vertexAttrs[2] = {
  //     .location = 2,
  //     .binding = 0,
  //     .format = VK_FORMAT_R32G32_SFLOAT,
  //     .offset = offsetof(Vertex, texCoord),
  // };

  // VkVertexInputBindingDescription vertexBinding{
  //     .binding = 0,
  //     .stride = sizeof(Vertex),
  //     .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  // };

  VkPipelineVertexInputStateCreateInfo vertexInput{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      // .vertexBindingDescriptionCount = 1,
      // .pVertexBindingDescriptions = &vertexBinding,
      // .vertexAttributeDescriptionCount = vertexAttrs.size(),
      // .pVertexAttributeDescriptions = vertexAttrs.data(),
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
      .cullMode = VK_CULL_MODE_NONE,
      .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
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
      .stageCount = 2,
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
}  // namespace vk1