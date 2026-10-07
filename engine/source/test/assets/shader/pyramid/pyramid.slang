struct VSInput {
  float3 pos;
  float2 uv;
  float3 normal;
  float3 tangent;
};

struct UniformBufferObject {
  float4x4 model;
  float4x4 view;
  float4x4 proj;
};

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
  out.color = float4(input.normal, 1.0f);
  out.texCoord = input.uv;
  return out;
}

[shader("fragment")]
float4 fragMain(VSOutput inVert)
    : SV_Target {
  // return texture.Sample(inVert.texCoord);
  return inVert.color;
}
