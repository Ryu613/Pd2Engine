module SceneBindings;

import Scene.SceneTypes;

// set = 0, binding = 0
[[vk::binding(0, 0)]]
public ConstantBuffer<CameraData> uCameraData;

// set = 1, binding = 0
[[vk::binding(0, 1)]]
public StructuredBuffer<ObjectData> sObjectData;
