struct VSInput {
  float3 position;
  float3 color;
  float2 texCoord;
};

struct UBO {
  float4x4 model;
  float4x4 view;
  float4x4 proj;
};

[vk::binding(0, 0)]
ConstantBuffer<UBO> mvp;

struct VSOutput {
  float4 sv_position : SV_Position;
  float4 color;
  float2 texCoord;
}

Sampler2D texture;

[shader("vertex")]
VSOutput vertMain(VSInput input, uint vid: SV_VertexID) {
  VSOutput out;
  out.sv_position = mul(mvp.proj, mul(mvp.view, mul(mvp.model, float4(input.position, 1.0f))));
  out.color = float4(input.color, 1.0f);
  out.texCoord = input.texCoord;
  return out;
}

[shader("fragment")]
float4 fragMain(VSOutput inVert)
    : SV_Target {
  return texture.Sample(inVert.texCoord);
}
