module Core.io;

public struct VSInput {
  [[vk::location(0)]]
  float3 pos;
  [[vk::location(1)]]
  float2 uv;
  [[vk::location(2)]]
  float3 normal;
  [[vk::location(3)]]
  float3 tangent;
};
