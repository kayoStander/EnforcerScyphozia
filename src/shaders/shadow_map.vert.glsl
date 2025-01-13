#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(constant_id = 0) const int pointLightsAmount=10;

struct PointLight {
  vec4 position;
  vec4 color;
};
layout(set = 0, binding = 0) uniform GlobalUniformBufferObject{
  mat4 projection;
  mat4 view;
  mat4 inverseView;
  vec4 ambientLightColor;
  PointLight pointLights[pointLightsAmount];
  float time;
  int numLights;
} uniformBufferObject;

layout(location = 0) out vec4 fragPosLightSpace;

void main(){
  vec4 worldPosition = vec4(position, 1.0);
  fragPosLightSpace = worldPosition;
  gl_Position = fragPosLightSpace;
}
