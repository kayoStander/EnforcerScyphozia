#version 450

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

struct PointLight {
  vec4 position;
  vec4 color;
};
layout(set = 0, binding = 0) uniform GlobalUniformBufferObject{
  mat4 projection;
  mat4 view;
  mat4 inverseView;
  vec4 ambientLightColor;
  PointLight pointLights[10];
  int numLights;
} uniformBufferObject;
layout(set = 0, binding = 1) uniform sampler2D images[5];
layout(set = 0, binding = 2) uniform samplerCube environmentMap;
layout(push_constant) uniform Push {
  vec3 fogColor;
  float fogStart;
  float fogEnd;
} push;

void main() {
  vec2 uv = fragUV * 2.0 - 1.0;
  vec3 viewDirection = normalize(vec3(uv.x, uv.y, -1.0));

  vec3 worldDirection = normalize((mat4(mat3(uniformBufferObject.inverseView)) * vec4(viewDirection,0.)).xyz);

  outColor = texture(images[4],fragUV);
}
