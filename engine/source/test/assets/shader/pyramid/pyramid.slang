struct VSInput {
  float3 pos;
  float3 normal;
  float2 uv;
};

struct UniformBufferObject {
  float4x4 model;
  float4x4 view;
  float4x4 proj;
};

// static float4 vertices[5] =
//     float4[](float4(0.0f, 0.5f, 0.0f, 1.0f), float4(-0.5f, -0.5f, -0.5f, 1.0f), float4(0.5f, -0.5f, -0.5f, 1.0f),
//              float4(0.5f, -0.5f, 0.5f, 1.0f), float4(-0.5f, -0.5f, 0.5f, 1.0f), );

// static float4 colors[5] =
//     float4[](float4(1.0f, 0.0f, 1.0f, 1.0f), float4(0.0f, 1.0f, 1.0f, 1.0f), float4(0.0f, 0.0f, 1.0f, 1.0f),
//              float4(0.0f, 1.0f, 0.0f, 1.0f), float4(1.0f, 1.0f, 0.0f, 1.0f), );

// [vk::binding(0, 0)]
// ConstantBuffer<UBO> mvp;

struct VSOutput {
  float4 sv_position : SV_Position;
  float4 color;
  float2 texCoord;
}

// Sampler2D texture;

[shader("vertex")]
VSOutput vertMain(VSInput input, uniform UniformBufferObject* ubo, uint vid: SV_VertexID) {
  VSOutput out;
  out.sv_position = mul(ubo->proj, mul(ubo->view, mul(ubo->model, float4(input.pos, 1.0f))));
  // out.sv_position = mul(ubo->view, mul(ubo->model, float4(input.pos, 1.0f)));
      // out.sv_position = float4(input.pos, 1.0f);
  out.color = float4(input.normal, 1.0f);
  // out.color = float4(abs(ubo->proj[2][3]), abs(ubo->proj[3][2]) * 5.0f, 0, 1.0f);
  out.texCoord = input.uv;
  return out;
}

[shader("fragment")]
float4 fragMain(VSOutput inVert)
    : SV_Target {
  // return texture.Sample(inVert.texCoord);
  return inVert.color;
}
