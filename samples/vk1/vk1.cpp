#include "vk1_initializer.hpp"
#include "SDL3/SDL.h"

#ifndef ASSET_DIR
#define ASSET_DIR ./
#endif

VkShaderModule loadShaderModule(VkDevice device, const char* path) {
  FILE* file = fopen(path, "rb");
  assert(file);
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  assert(length >= 0);
  fseek(file, 0, SEEK_SET);
  char* buffer = new char[length];
  assert(buffer);
  size_t rc = fread(buffer, 1, length, file);
  assert(rc == size_t(length));
  fclose(file);

  assert(length % 4 == 0);

  uint32_t codeSize = length;

  VkShaderModuleCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = codeSize,
      .pCode = reinterpret_cast<const uint32_t*>(buffer),
  };

  VkShaderModule shaderModule = 0;
  vk1::checkResult(vkCreateShaderModule(device, &createInfo, 0, &shaderModule));

  return shaderModule;
}

VkPipelineLayout createPipelineLayout(VkDevice device) {
  VkPipelineLayoutCreateInfo layoutCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
  };
  VkPipelineLayout layout = 0;
  vk1::checkResult(vkCreatePipelineLayout(device, &layoutCreateInfo, 0, &layout));

  return layout;
}

VkPipeline createGraphicsPipeline(VkDevice device, VkPipelineCache cache, VkPipelineLayout layout,
                                  VkRenderPass renderpass, VkShaderModule vert, VkShaderModule frag) {
  std::vector<VkPipelineShaderStageCreateInfo> stageCreateInfo{2};
  stageCreateInfo[0] = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vert,
      .pName = "main",
  };

  stageCreateInfo[1] = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = frag,
      .pName = "main",
  };

  VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
  };
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
  };
  VkPipelineViewportStateCreateInfo viewportCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .scissorCount = 1,
  };
  VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .lineWidth = 1.0f,
  };
  VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
  };

  VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
  };
  std::vector<VkPipelineColorBlendAttachmentState> colorAttachmentState{1};
  colorAttachmentState[0] = {
      .colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };
  VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments = colorAttachmentState.data(),
  };
  std::vector<VkDynamicState> dynamicStates{2};
  dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
  dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;
  VkPipelineDynamicStateCreateInfo dynamicCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = 2,
      .pDynamicStates = dynamicStates.data(),
  };
  VkGraphicsPipelineCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = static_cast<uint32_t>(stageCreateInfo.size()),
      .pStages = stageCreateInfo.data(),
      .pVertexInputState = &vertexInputCreateInfo,
      .pInputAssemblyState = &inputAssemblyCreateInfo,
      .pViewportState = &viewportCreateInfo,
      .pRasterizationState = &rasterizationCreateInfo,
      .pMultisampleState = &multisampleCreateInfo,
      .pDepthStencilState = &depthStencilCreateInfo,
      .pColorBlendState = &colorBlendCreateInfo,
      .pDynamicState = &dynamicCreateInfo,
      .layout = layout,
      .renderPass = renderpass,
  };
  VkPipeline pipeline = 0;
  vk1::checkResult(vkCreateGraphicsPipelines(device, cache, 1, &createInfo, 0, &pipeline));

  return pipeline;
}

int main() {
  if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
    return -1;
  }
  int width = 1024;
  int height = 768;
  SDL_Window* window = SDL_CreateWindow("vk1_test", width, height,
                                        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  if (window == nullptr) {
    return -1;
  }
  void* nativeWindowHandle =
      SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

  vk1::Vk1Device device = vk1::Vk1Initializer::Builder()  //
                              .enableDebug()
                              .enableSurface()
                              .enableBindless()
                              .build();

  device.createSwapchain(nativeWindowHandle, width, height);

  auto vkDevice = device.getDevice();
  auto vkSwapchain = device.getSwapchain();

  VkCommandPool cmdPool = device.createCommandPool();
  assert(cmdPool);

  VkCommandBufferAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = cmdPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };
  VkCommandBuffer cmdBuffer = 0;
  vkAllocateCommandBuffers(vkDevice, &allocInfo, &cmdBuffer);

  auto renderpass = device.createRenderPass();
  auto frameBuffers = device.createFramebuffers(renderpass);

  VkShaderModule triangleVert = loadShaderModule(vkDevice, ASSET_DIR "shaders/triangle.vert.spv");
  assert(triangleVert);
  VkShaderModule triangleFrag = loadShaderModule(vkDevice, ASSET_DIR "shaders/triangle.frag.spv");
  assert(triangleFrag);

  VkPipelineCache pipelineCache = 0;

  VkPipelineLayout layout = createPipelineLayout(vkDevice);

  VkPipeline trianglePipeline =
      createGraphicsPipeline(vkDevice, pipelineCache, layout, renderpass, triangleVert, triangleFrag);

  vkDestroyShaderModule(vkDevice, triangleVert, 0);
  vkDestroyShaderModule(vkDevice, triangleFrag, 0);

  VkSemaphore acquireSemaphore = device.createSemaphore();
  assert(acquireSemaphore);

  auto presentSemaphores = device.getSwapchainInfo().presentSemaphores;

  uint32_t uWidth = width;
  uint32_t uheight = height;
  auto presentQueue = device.getQueueInfo().presentQueue;

  bool windowClose = false;
  while (!windowClose) {
    for (SDL_Event event; SDL_PollEvent(&event);) {
      // quit window
      if (event.type == SDL_EVENT_QUIT) {
        windowClose = true;
        break;
      }
      if (event.type == SDL_EVENT_KEY_DOWN) {
        // 按下esc退出
        if (event.key.key == SDLK_ESCAPE) {
          windowClose = true;
          break;
        }
      }
    }
    VkAcquireNextImageInfoKHR acquireInfo{
        .sType = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR,
        .swapchain = vkSwapchain,
    };
    uint32_t imageIndex = device.acquireNextImage(acquireSemaphore);
    vk1::checkResult(vkResetCommandPool(vkDevice, cmdPool, 0));

    VkCommandBufferBeginInfo BufferBeginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    vk1::checkResult(vkBeginCommandBuffer(cmdBuffer, &BufferBeginInfo));

    VkClearValue color{
        // in my rtx 4070s driver, this is bgra
        .color = {36.0f / 255.0f, 10.0f / 255.0f, 48.0f / 255.0f, 1.0f},
    };

    VkRenderPassBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderpass,
        .framebuffer = frameBuffers[imageIndex],
        .renderArea =
            {
                .offset =
                    {
                        .x = 0,
                        .y = 0,
                    },
                .extent =
                    {
                        .width = uWidth,
                        .height = uheight,
                    },

            },
        .clearValueCount = 1,
        .pClearValues = &color,
    };

    vkCmdBeginRenderPass(cmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{
        .x = 0,
        .y = static_cast<float>(height),
        .width = static_cast<float>(width),
        .height = -static_cast<float>(height),
        .minDepth = 0,
        .maxDepth = 1,
    };
    VkRect2D scissor{
        .offset =
            {
                .x = 0,
                .y = 0,
            },
        .extent =
            {
                .width = uWidth,
                .height = uheight,
            },

    };

    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
    // draw calls
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, trianglePipeline);
    vkCmdDraw(cmdBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(cmdBuffer);

    vk1::checkResult(vkEndCommandBuffer(cmdBuffer));

    VkPipelineStageFlags submitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &acquireSemaphore,  // 提交前要等待的semaphore，此处意思就是需要swapchain
                                               // acquire image signal才能提交
        .pWaitDstStageMask = &submitStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmdBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &presentSemaphores[imageIndex],  // 提交后要通知的semaphore，此处意思是让cmd
                                                              // buffer提交完成以后要通知的semaphore
    };
    vkQueueSubmit(presentQueue, 1, &submitInfo, 0);

    device.present(imageIndex, presentSemaphores[imageIndex]);

    device.waitIdle();
  }

  device.destroySemaphore(acquireSemaphore);
  device.destroyPipeline(trianglePipeline);
  device.destroyPipelineLayout(layout);
  for (auto eachframeBuffer : frameBuffers) {
    device.destroyFramebuffer(eachframeBuffer);
  }
  device.destroyRenderPass(renderpass);
  device.destroyCommandPool(cmdPool);

  SDL_DestroyWindow(window);

  return 0;
}
