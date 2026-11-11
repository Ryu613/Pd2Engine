struct VSInput {
  float3 pos;
  float2 uv;
  float3 normal;
  float3 tangent;
};

struct VSOutput {
  float4 position : SV_Position;
  float4 color;
  float2 uv;
  uint32_t instanceIndex;
};

struct CameraData {
  float4x4 view;
  float4x4 proj;
};

struct ObjectData {
  float4x4 model;
};

[[vk::binding(0, 0)]]
ConstantBuffer<CameraData> uCameraData;
[[vk::binding(0, 1)]]
StructuredBuffer<ObjectData> sObjectData;

// Sampler2D texture;

[shader("vertex")]
VSOutput vertMain(VSInput input, uint instanceIndex: SV_VulkanInstanceID) {
  VSOutput output;
  ObjectData objectData = sObjectData[instanceIndex];
  output.position = mul(uCameraData.proj, mul(uCameraData.view, mul(objectData.model, float4(input.pos, 1.0f))));
  output.color = float4(input.normal, 1.0f);
  output.uv = input.uv;
  output.instanceIndex = instanceIndex;
  return output;
}

[shader("fragment")]
float4 fragMain(VSOutput inVert)
    : SV_Target {
  // return texture.Sample(inVert.texCoord);
  return inVert.color;
}
