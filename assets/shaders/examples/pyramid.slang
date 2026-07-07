static float4 vertices[5] =
    float4[](float4(0.0f, 0.5f, 0.0f, 1.0f), float4(-0.5f, -0.5f, -0.5f, 1.0f),
             float4(0.5f, -0.5f, -0.5f, 1.0f), float4(0.5f, -0.5f, 0.5f, 1.0f),
             float4(-0.5f, -0.5f, 0.5f, 1.0f), );
static float4 colors[5] = float4[](float4(1.0f, 0.0f, 1.0f, 1.0f), float4(0.0f, 1.0f, 1.0f, 1.0f),
                                   float4(0.0f, 0.0f, 1.0f, 1.0f), float4(0.0f, 1.0f, 0.0f, 1.0f),
                                   float4(1.0f, 1.0f, 0.0f, 1.0f), );

struct MVP {
  float4x4 model;
  float4x4 view;
  float4x4 proj;
};

[vk::binding(0, 0)]
ConstantBuffer<MVP> mvp;

struct VSOutput {
  float4 sv_position : SV_Position;
  float4 color;
};

[shader("vertex")]
VSOutput vertMain(uint vid: SV_VertexID) {
  VSOutput out;
  out.sv_position = mul(mvp.proj, mul(mvp.view, mul(mvp.model, vertices[vid])));
  out.color = colors[vid];
  return out;
}

[shader("fragment")]
float4 fragMain(VSOutput inVert)
    : SV_Target {
  return inVert.color;
}
